#!/usr/bin/python

import argparse
import os
import re
import sys
import subprocess

PREFIX = """\
<?xml version="1.0" encoding="ISO-8859-1" standalone="no"?>
<?modxslt-stylesheet type="text/xsl" method="http" href="${xsltRoot}/translator.xsl"
        media="screen and ! $GET[format]" alternate="no" title="For any web browser" charset="ISO-8859-1"?>
<?xml-stylesheet type="text/xsl" method="http" href="../../lib/xslt/sdope/docbook.xsl"
        media="screen" alternate="no" title="For any web browser" charset="ISO-8859-1"?>"""

def RunCommand(command, **args):
  """Runs a command.""" 
  # stdout=subprocess.PIPE
  print "* Running command '%s'" % command
  process = subprocess.Popen(command, **args)
  stdout, stderr = process.communicate()
  return process.wait(), stdout, stderr

def RunCommandOrExit(command, **args):
  status, stdout, stderr = RunCommand(command, **args)
  if status:
    print >>sys.stderr, "FAILED COMMAND, status", status
    sys.exit(status)
  return stdout, stderr

def NormalizeAndVerifyDir(directory, expected):
  directory = os.path.realpath(directory)
  check = os.path.join(directory, expected)
  if not os.path.exists(check):
    raise ValueError(
        "invalid directory, can't find %s - expected file not found %s" %
        (directory, check))
  return directory

def UpdateManualXML(source, destination):
  print "*** Updating manual, input %s, output %s" % (source, destination)
  manual = open(source).read()
  output = open(destination, "w")

  manual = re.sub(r"^<!DOCTYPE.*$", "", manual, flags=re.MULTILINE)
  manual = re.sub(r"<Article>",
                  r"<Article xmlns='http://www.masobit.net/ns/sdope/1.0'>",
                  manual, flags=re.MULTILINE)
  print >>output, PREFIX
  print >>output, manual
  output.close()

def GitHasStagedChanges(directory):
  status, stdout, stderr = RunCommand(
      ["git", "diff-index", "--quiet", "--cached", "HEAD"],
      cwd=directory)
  return bool(status)

def GitDirectoryGetPendingChanges(directory):
  changed, error = RunCommandOrExit(
      ["git", "status", "--porcelain", "./"], cwd=directory,
      stdout=subprocess.PIPE)
  return changed, error

def GitCheckInChangesInDirectory(directory):
  RunCommandOrExit(["git", "add", "./"], cwd=directory)
  RunCommandOrExit(["git", "commit", "-e", "-m",
                    "Automatically updated documentation."], cwd=directory)

def GitPushChangesInDirectory(directory):
  RunCommandOrExit(["git", "push"], cwd=directory)

def GitPullChangesInDirectory(directory):
  RunCommandOrExit(["git", "pull"], cwd=directory)

def PromptYesOrNo(message):
  try: input = raw_input
  except NameError: pass

  warning = ""
  while True:
    value = input(warning + message + " [yes/no]: ")
    value = value.lower().strip()
    if value == "yes":
      return True
    if value == "no":
      return False
    warning = "Must be yes or no. "

def main(argv):
  parser = argparse.ArgumentParser(description="Blah Blah Blah")
  parser.add_argument(
    "-s", "--source-dir", dest="source_dir", default=".",
    help="Top level directory where modxslt source code can be found")
  parser.add_argument(
    "-o", "--output-dir", dest="output_dir", required=True,
    help="Directory where to install the documentation")
  parser.add_argument(
    "-f", "--force", dest="force", action="store_true",
    help="Directory where to install the documentation")

  args = parser.parse_args()

  # Normalize all paths, and make sure they are correct.
  try:
    args.source_dir = NormalizeAndVerifyDir(
        args.source_dir, "configure.ac")
    args.output_dir = NormalizeAndVerifyDir(
        args.output_dir, "http/doc/documentation.xml")
    doc_source_base_dir = NormalizeAndVerifyDir(
        os.path.join(args.source_dir, "doc"), "Makefile")
    doc_source_manual_dir = NormalizeAndVerifyDir(
        os.path.join(args.source_dir, "doc/manual"), "manual.xml")

    doc_output_base_dir = NormalizeAndVerifyDir(
        os.path.join(args.output_dir, "http/doc"), "documentation.xml")
    doc_output_download_dir = NormalizeAndVerifyDir(
        os.path.join(args.output_dir, "http/doc/download"), "manual")
  except ValueError, message:
    parser.error(message)

  # Check that git repository is clean.
  if GitHasStagedChanges(args.source_dir):
    print >>sys.stderr, (
        "%s has staged changes, please commit" % args.source_dir)
    return 2
  if GitHasStagedChanges(args.output_dir):
    print >>sys.stderr, (
        "%s has staged changes, please commit" % args.output_dir)
    return 2

  GitPullChangesInDirectory(args.source_dir)
  GitPullChangesInDirectory(args.output_dir)

  # Finally start updating all docs.
  print "*** Updating all documents..."
  RunCommandOrExit("make all", cwd=doc_source_base_dir, shell=True)

  # Check if documents have source_changed, and if we want to check in the change.
  source_changed, stderr = GitDirectoryGetPendingChanges(doc_source_base_dir)
  if not source_changed and not args.force:
    print "*** Nothing changed in source directory, exiting"
    return 0

  # Copy all files from all directory to new one.
  RunCommandOrExit(
      ["rsync", "-avz", "--delete", "--exclude=Makefile",
       doc_source_base_dir + "/", doc_output_download_dir + "/"])

  # Update manual.xml file.
  UpdateManualXML(os.path.join(doc_source_manual_dir, "manual.xml"),
                  os.path.join(doc_output_base_dir, "manual.xml"))

  destination_changed, stderr = GitDirectoryGetPendingChanges(doc_output_base_dir)

  if source_changed and PromptYesOrNo("Check in changes in %s" % doc_source_base_dir):
    GitCheckInChangesInDirectory(doc_source_base_dir)
    if PromptYesOrNo("Push changes in %s" % doc_source_base_dir):
      GitPushChangesInDirectory(doc_source_base_dir)

  if destination_changed and PromptYesOrNo("Check in changes in %s" % doc_output_base_dir):
    GitCheckInChangesInDirectory(doc_output_base_dir)
    if PromptYesOrNo("Push changes in %s" % doc_output_base_dir):
      GitPushChangesInDirectory(doc_output_base_dir)

if __name__ == "__main__":
  sys.exit(main(sys.argv))
