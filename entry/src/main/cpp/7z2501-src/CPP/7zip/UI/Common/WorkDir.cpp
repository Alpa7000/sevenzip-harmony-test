// WorkDir.cpp

#include "StdAfx.h"

#include "../../../Windows/FileName.h"
#include "../../../Windows/FileSystem.h"

#include "WorkDir.h"

using namespace NWindows;
using namespace NFile;
using namespace NDir;

FString GetWorkDir(const NWorkDir::CInfo &workDirInfo, const FString &path, FString &fileName)
{
  NWorkDir::NMode::EEnum mode = workDirInfo.Mode;
  
  #if defined(_WIN32) && !defined(UNDER_CE)
  if (workDirInfo.ForRemovableOnly)
  {
    mode = NWorkDir::NMode::kCurrent;
    const FString prefix = path.Left(3);
    if (NName::IsDrivePath(prefix))
    {
      const UINT driveType = NSystem::MyGetDriveType(prefix);
      if (driveType == DRIVE_CDROM || driveType == DRIVE_REMOVABLE)
        mode = workDirInfo.Mode;
    }
    /*
    CParsedPath parsedPath;
    parsedPath.ParsePath(archiveName);
    UINT driveType = GetDriveType(parsedPath.Prefix);
    if ((driveType != DRIVE_CDROM) && (driveType != DRIVE_REMOVABLE))
      mode = NZipSettings::NWorkDir::NMode::kCurrent;
    */
  }
  #endif
  
  const int pos = path.ReverseFind_PathSepar() + 1;
  fileName = path.Ptr((unsigned)pos);
  
  FString tempDir;
  switch ((int)mode)
  {
    case NWorkDir::NMode::kCurrent:
      tempDir = path.Left((unsigned)pos);
      break;
    case NWorkDir::NMode::kSpecified:
      tempDir = workDirInfo.Path;
      break;
    // case NWorkDir::NMode::kSystem:
    default:
      if (!MyGetTempPath(tempDir))
        throw 141717;
      break;
  }
  NName::NormalizeDirPathPrefix(tempDir);
  return tempDir;
}

HRESULT CWorkDirTempFile::CreateTempFile(const FString &originalPath)
{
  NWorkDir::CInfo workDirInfo;
  workDirInfo.Load();
  FString namePart;
  FString path = GetWorkDir(workDirInfo, originalPath, namePart);
  CreateComplexDir(path);
  path += namePart;
  _outStreamSpec = new COutFileStream;
  OutStream = _outStreamSpec;
  if (!_tempFile.Create(path, &_outStreamSpec->File))
    return GetLastError_noZero_HRESULT();
  _originalPath = originalPath;
  return S_OK;
}

HRESULT CWorkDirTempFile::MoveToOriginal(bool deleteOriginal,
    NWindows::NFile::NDir::ICopyFileProgress *progress)
{
  OutStream.Release();
  if (!_tempFile.MoveTo(_originalPath, deleteOriginal, progress))
    return GetLastError_noZero_HRESULT();
  return S_OK;
}

// 添加非 Windows 平台的 Load 实现（与头文件声明 void 一致）
#ifndef _WIN32
namespace NWorkDir {
  void CInfo::Load() {
    // 非 Windows 环境不需要加载注册表等操作，留空即可
  }
}
#endif