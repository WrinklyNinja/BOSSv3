#!/usr/bin/env node
// Archive packaging script. Takes one argument, which is the path to the
// repository's root. Requires 7-zip and Git to be installed, and Git to be
// available on the system path.
'use strict';
const childProcess = require('child_process');
const path = require('path');
const fs = require('fs-extra');
const os = require('os');
const helpers = require('./helpers');

function vulcanize(rootPath) {
  return childProcess.execFileSync('node', [
    'scripts/vulcanize.js',
    rootPath,
  ]);
}

function getGitDescription() {
  const describe = String(childProcess.execFileSync('git', [
    'describe',
    '--tags',
    '--long',
  ])).slice(0, -1);
  let branch = String(childProcess.execFileSync('git', [
    'rev-parse',
    '--abbrev-ref',
    'HEAD',
  ])).slice(0, -1);

  /* On AppVeyor and Travis CI, a specific commit is checked out, so the branch
     is HEAD. Use their stored branch value instead. */
  if (branch === 'HEAD') {
    if (process.env.APPVEYOR_REPO_BRANCH) {
      branch = process.env.APPVEYOR_REPO_BRANCH;
    } else if (process.env.TRAVIS_BRANCH) {
      branch = process.env.TRAVIS_BRANCH;
    }
  }

  return `${describe}_${branch}`;
}

function compress(sourcePath, destPath) {
  // First remove any existing archive.
  fs.removeSync(destPath);

  const filename = path.basename(destPath);
  const rootFolder = path.basename(sourcePath);
  const workingDirectory = path.dirname(sourcePath);

  if (os.platform() === 'win32') {
    let sevenzipPath = path.join('C:\\', 'Program Files', '7-Zip', '7z.exe');
    if (!helpers.fileExists(sevenzipPath)) {
      sevenzipPath = '7z';
    }

    // The last argument must have a leading dot for the subdirectory not to
    // be present in the archive, but path.join removes it, so it's prefixed.
    return childProcess.execFileSync(sevenzipPath, [
      'a',
      '-r',
      filename,
      rootFolder,
    ], {
      cwd: workingDirectory,
    });
  }

  return childProcess.execSync(`tar -cJf ${filename} ${rootFolder}`, {
    cwd: workingDirectory,
  });
}

function createAppArchive(rootPath, releasePath, tempPath, destPath) {
  // Ensure that the output directory is empty.
  fs.emptyDirSync(tempPath);

  // Copy LOOT exectuable and CEF files.
  let binaries = [];
  if (os.platform() === 'win32') {
    binaries = [
      'LOOT.exe',
      'd3dcompiler_47.dll',
      'libEGL.dll',
      'libGLESv2.dll',
      'libcef.dll',
      'natives_blob.bin',
      'snapshot_blob.bin',
      'cef.pak',
      'cef_100_percent.pak',
      'cef_200_percent.pak',
      'devtools_resources.pak',
      'icudtl.dat',
    ];
  } else {
    binaries = [
      'LOOT',
      'chrome-sandbox',
      'libcef.so',
      'natives_blob.bin',
      'snapshot_blob.bin',
      'cef.pak',
      'cef_100_percent.pak',
      'cef_200_percent.pak',
      'devtools_resources.pak',
      'icudtl.dat',
    ];
  }
  binaries.forEach((file) => {
    fs.copySync(
      path.join(releasePath, file),
      path.join(tempPath, file)
    );
  });

  // CEF locale file.
  fs.mkdirsSync(path.join(tempPath, 'resources', 'l10n'));
  fs.copySync(
    path.join(releasePath, 'resources', 'l10n', 'en-US.pak'),
    path.join(tempPath, 'resources', 'l10n', 'en-US.pak')
  );

  // Translation files.
  [
    'es', 'ru', 'fr', 'zh_CN', 'pl', 'pt_BR', 'fi', 'de', 'da', 'ko',
  ].forEach((lang) => {
    fs.mkdirsSync(path.join(tempPath, 'resources', 'l10n', lang, 'LC_MESSAGES'));
    fs.copySync(
      path.join(rootPath, 'resources', 'l10n', lang, 'LC_MESSAGES', 'loot.mo'),
      path.join(tempPath, 'resources', 'l10n', lang, 'LC_MESSAGES', 'loot.mo')
    );
  });

  // UI files.
  fs.mkdirsSync(path.join(tempPath, 'resources', 'ui', 'css'));
  fs.copySync(
    path.join(releasePath, 'resources', 'ui', 'index.html'),
    path.join(tempPath, 'resources', 'ui', 'index.html')
  );
  fs.copySync(
    path.join(rootPath, 'resources', 'ui', 'css', 'dark-theme.css'),
    path.join(tempPath, 'resources', 'ui', 'css', 'dark-theme.css')
  );
  fs.copySync(
    path.join(rootPath, 'resources', 'ui', 'fonts'),
    path.join(tempPath, 'resources', 'ui', 'fonts')
  );

  // Docs.
  fs.mkdirsSync(path.join(tempPath, 'docs'));
  [
    'images',
    'licenses',
    'LOOT Readme.html',
  ].forEach((item) => {
    fs.copySync(
      path.join(rootPath, 'docs', item),
      path.join(tempPath, 'docs', item)
    );
  });

  // Now compress the folder to a 7-zip archive.
  compress(tempPath, destPath);

  // Finally, delete the temporary folder.
  fs.removeSync(tempPath);
}

function createApiArchive(rootPath, releasePath, tempPath, destPath) {
  // Ensure that the output directory is empty.
  fs.emptyDirSync(tempPath);

  // API binary/binaries.
  let binaries = [];
  if (os.platform() === 'win32') {
    binaries = [
      'loot_api.dll',
      'loot_api.lib',
    ];
  } else {
    binaries = [
      'libloot_api.so',
    ];
  }
  binaries.forEach((file) => {
    fs.copySync(
      path.join(releasePath, file),
      path.join(tempPath, file)
    );
  });

  // API header files.
  fs.mkdirsSync(path.join(tempPath, 'include'));
  fs.copySync(
    path.join(rootPath, 'include', 'loot'),
    path.join(tempPath, 'include', 'loot')
  );

  // Readme.
  fs.copySync(
    path.join(rootPath, 'docs', 'api', 'README.md'),
    path.join(tempPath, 'README.md')
  );

  // Now compress the folder to a 7-zip archive.
  compress(tempPath, destPath);

  // Finally, delete the temporary folder.
  fs.removeSync(tempPath);
}

function createMetadataValidatorArchive(rootPath, binaryPath, tempPath, destPath) {
  // Ensure that the output directory is empty.
  fs.emptyDirSync(tempPath);

  // Metadata validator binary.
  fs.copySync(
    binaryPath,
    path.join(tempPath, path.basename(binaryPath))
  );

  // Now compress the folder to a 7-zip archive.
  compress(tempPath, destPath);

  // Finally, delete the temporary folder.
  fs.removeSync(tempPath);
}

function getFilenameSuffix(label, gitDescription) {
  if (label) {
    return `${gitDescription}_${label}`;
  }

  return `${gitDescription}`;
}

function getArchiveFileExtension() {
  if (os.platform() === 'win32') {
    return '.7z';
  }

  return '.tar.xz';
}

let rootPath = '.';
if (process.argv.length > 2) {
  rootPath = process.argv[2];
}

const gitDesc = getGitDescription();
const fileExtension = getArchiveFileExtension();
vulcanize(rootPath);

helpers.getAppReleasePaths(rootPath).forEach(releasePath => {
  const filename = `loot_${getFilenameSuffix(releasePath.label, gitDesc)}`;
  createAppArchive(rootPath,
                   releasePath.path,
                   path.join(rootPath, 'build', filename),
                   path.join(rootPath, 'build', filename + fileExtension));
});

helpers.getApiBinaryPaths(rootPath).forEach(binaryPath => {
  const filename = `loot-api_${getFilenameSuffix(binaryPath.label, gitDesc)}`;
  createApiArchive(rootPath,
                   binaryPath.path,
                   path.join(rootPath, 'build', filename),
                   path.join(rootPath, 'build', filename + fileExtension));
});

helpers.getMetadataValidatorBinaryPaths(rootPath).forEach(binaryPath => {
  const filename = `metadata-validator_${getFilenameSuffix(binaryPath.label, gitDesc)}`;
  createMetadataValidatorArchive(rootPath,
                                 binaryPath.path,
                                 path.join(rootPath, 'build', filename),
                                 path.join(rootPath, 'build', filename + fileExtension));
});
