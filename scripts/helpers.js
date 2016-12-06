// Helper functions shared across scripts.
'use strict';
const childProcess = require('child_process');
const path = require('path');
const fs = require('fs');
const os = require('os');

function fileExists(filePath) {
  try {
    // Query the entry
    const stats = fs.lstatSync(filePath);

    // Is it a directory?
    if (stats.isFile()) {
      return true;
    }
  } catch (e) {
    /* Don't do anything, it's not an error. */
  }

  return false;
}

function getBinaryParentPaths(rootPath) {
  const paths = [
    {
      path: path.join(rootPath, 'build'),
      label: null,
    },
    {
      path: path.join(rootPath, 'build', '32'),
      label: '32-bit',
    },
    {
      path: path.join(rootPath, 'build', '64'),
      label: '64-bit',
    },
  ];

  if (os.platform() === 'win32') {
    paths.forEach(parentPath => {
      parentPath.path = path.join(parentPath.path, 'Release');
    });
  }

  return paths;
}

function getAppReleasePaths(rootPath) {
  let file = 'LOOT';
  if (os.platform() === 'win32') {
    file += '.exe';
  }

  return getBinaryParentPaths(rootPath).filter(
    parentPath => fileExists(path.join(parentPath.path, file))
  );
}

function getBinaryPaths(rootPath, file) {
  return getBinaryParentPaths(rootPath)
    .map(parentPath => {
      parentPath.path = path.join(parentPath.path, file);
      return parentPath;
    })
    .filter(parentPath => fileExists(parentPath.path));
}

function getApiBinaryPaths(rootPath) {
  let file = 'loot_api';
  if (os.platform() === 'win32') {
    file += '.dll';
  } else {
    file = `lib${file}.so`;
  }

  return getBinaryParentPaths(rootPath).filter(
    parentPath => fileExists(path.join(parentPath.path, file))
  );
}

function getMetadataValidatorBinaryPaths(rootPath) {
  let file = 'metadata-validator';
  if (os.platform() === 'win32') {
    file += '.exe';
  }

  return getBinaryPaths(rootPath, file);
}

function safeExecFileSync(file, args, options) {
  try {
    return childProcess.execFileSync(file, args, options);
  } catch (error) {
    throw new Error(error.message);
  }
}

module.exports.fileExists = fileExists;
module.exports.getAppReleasePaths = getAppReleasePaths;
module.exports.getApiBinaryPaths = getApiBinaryPaths;
module.exports.getMetadataValidatorBinaryPaths = getMetadataValidatorBinaryPaths;
module.exports.safeExecFileSync = safeExecFileSync;
