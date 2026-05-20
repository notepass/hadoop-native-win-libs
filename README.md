# Native windows libraries for hadoop
This repository contains a pipeline which builds the Windows native libraries for Hadoop (winutils.exe and hadoop.dll)
which are not distributed with hadoop.  
Unlike other repos where the binaries are just uploaded to use, this repository aims to build the binaries via GitHub
actions, thus making the build process transparent.

*The download of the build binaries is possible via the "releases" section.*

## Important: WinSDK compatibility shim in the build pipeline
Modern Windows SDK versions declare `GetFileInformationByName` in `fileapi.h`, while Hadoop's
`winutils` code also declares a function with the same C symbol name but a different signature.
That combination breaks the build (`C2733` during compile, and depending on workaround order also
`LNK2005` during link).

To keep upstream Hadoop sources untouched, this repository applies a build-time shim in
`.github/workflows/build-native-libs.yml`:

- a temporary header is force-included for `libwinutils` and `winutils` only
- `windows.h` is included first
- Hadoop's `GetFileInformationByName` references are macro-renamed to
  `Hadoop_GetFileInformationByName`

This avoids the SDK collision while preserving normal Hadoop runtime behavior (Hadoop usually uses
`winutils.exe` as a process, not direct linking to this symbol).

If you directly link against `libwinutils.lib` from external native code and expect an exported
symbol named `GetFileInformationByName`, note that this build will expose Hadoop's implementation
under `Hadoop_GetFileInformationByName` instead.

**The last release without this change is 3.4.0**

## How does this repo work?
For each release tag generated in the [apache/hadoop](https://github.com/apache/hadoop) repo, this repo will also
generate a tag. The creation of the tag will then trigger the build pipeline. This was done, so that the windows runner
will not be used for polling the source repo, as it consumes more credit per minute. Additionally, I do like my bash
over powershell. It also allows to push a tag manually to trigger a build.

The main build pipeline will pull the hadoop repo at the tag detected before. Afterwards the maven goals needed to
compile the binaries are called and a release containing the generated files is created.

The file "current-release.txt" is only utilised by the release checker pipeline. It takes track of the last release
that was detected, so that nothing is build multiple times. It plays no role for the main build pipeline, which will
only take the tag name into account. So for building the tag name can differ from the file contents without issue.

Because of architecture, the tag release might be visible a few minutes before the actual release. If a tag release 
exists for a prolonged time (30 minutes) without a binary release, the build has probably failed. In the future, issues
might be opened up automatically for failed builds.

## How long does it take for the binaries to be available after a hadoop release?
This repo checks at 6:00 (am) (probably UTC, I haven't read the documentation on cron actions) every day if there is a new
release. If so, it will build it. The build itself takes about 10 minutes. So 6:30 (am) after a release should be a good
time frame.

## Are release candidates also build?
Yes, release candidate tags are also build. They will be marked as pre-release.

## Is the trunk being build in a "nightly" fashion?
No, this isn't being done. If you need a trunk build, fork the repo, add the needed tokens as secrets
(See next section) and push a "trunk" tag. That should build the current trunk, tho I never tested it.

## How do I know that these binaries are not infected with a virus or poisoned in some other way?
The builds are run on the public GitHub runner infrastructure, utilising the Windows Server 2022 image with VS Enterprise.
You can check the pipeline definition and logs for each release to make sure nothing bad happened. Also fell free to
fork this repository and build the binaries yourself with this. Everything should be configured in a way that there are
no static references to this repository. You will need two tokens for this: ``RELEASE_SECRET`` which needs permissions
to create a release and ``REPO_SCOPED_TOKEN`` which is permitted to push to the repo.

## Why are there only builds for hadoop 3.3?
Versions before 3.3 had various bugs and dependencies I would need to work around in this pipeline.
For Versions 3.2 and earlier a "Command line too long" error can occur and versions 3.1 and earlier would require an
installation of protocolbuffer to build.

There is a nice [blog post](https://kontext.tech/article/378/compile-and-build-hadoop-321-on-windows-10-guide) regarding
these issues and possible fixes. Tho, I haven't implemented these and I am not planning to if these issues do not appear
in future releases.

## Is this repo open for contributions?
Yes, if you want to fix issues or just do some general cleanup open up a PR. I will be checking periodically.  
Please do refrain from senseless refactoring - Meaning changing structures or workflows for no other reason than "making
them nicer" or having them adhere to some weired rules. If a refactoring is needed for fixes or other measurable benefits
it is of course welcome. I just want to avoid the cargo cult.

## On backwards and forwards compatibility of the binaries
The parts of the repository containing the binaries for 
[winutils](https://github.com/apache/hadoop/tree/trunk/hadoop-common-project/hadoop-common/src/main/winutils)
and
[hadoop.dll](https://github.com/apache/hadoop/tree/trunk/hadoop-common-project/hadoop-common/src/main/native)
haven't seen actual code changes in quite a few years and probably won't anytime soon. So as long as you are not using
an extremely outdated version of hadoop/spark they should be compatible. This should also mean that you probably won't
need to update anytime soon.
