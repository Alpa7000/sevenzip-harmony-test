cmake_minimum_required(VERSION 3.15)
project(SevenZip VERSION 24.0 LANGUAGES C CXX ASM)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_FLAGS_RELEASE "")
set(CMAKE_CXX_FLAGS_RELEASE "")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 检测平台
if(WIN32)
    set(IS_MINGW ON)
    set(SHARED_EXT ".exe")
    set(CMAKE_RC_COMPILER_INIT windres)
    enable_language(RC)
else()
    set(IS_MINGW OFF)
    set(SHARED_EXT "")
endif()

# 编译器检查
if(CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(USE_CLANG ON)
endif()

# 检测架构
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(IS_X64 ON)
else()
    set(IS_X64 OFF)
endif()

# 设置基本编译选项
set(BASE_COMPILE_FLAGS "")
if(USE_CLANG)
    list(APPEND BASE_COMPILE_FLAGS -mbranch-protection=standard)
endif()

set(USE_ASM ON)

# 调试/发布模式
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(NDEBUG)
    set(DEBUG_FLAGS -g)
else()
    add_compile_definitions(NDEBUG)
    set(DEBUG_FLAGS -DNDEBUG)
endif()

# 警告选项
set(WARN_FLAGS -Wall -Wextra)

# 通用编译选项
add_compile_options(
    ${BASE_COMPILE_FLAGS}
    ${WARN_FLAGS}
    -DNDEBUG                       # 禁用断言
    -D_REENTRANT
    -D_FILE_OFFSET_BITS=64
    -D_LARGEFILE_SOURCE
    -fPIC
    -O3
    -march=armv8.2-a+crypto+fp16+rcpc
    -mtune=cortex-a76              # 针对麒麟 9000/8000 的 Cortex-A76/A77 优化
    -ftree-vectorize
    -fomit-frame-pointer
    -fstrict-aliasing
    -fno-semantic-interposition
    -pipe
    # 可选激进优化（若稳定可保留）
    -ffast-math
    -funroll-loops
)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

# ================== 源文件列表定义 ==================
set(COMMON_SOURCES
    ../../../Common/CommandLineParser.cpp
    ../../../Common/CRC.cpp
    ../../../Common/CrcReg.cpp
    ../../../Common/DynLimBuf.cpp
    ../../../Common/IntToString.cpp
    ../../../Common/Lang.cpp
    ../../../Common/ListFileUtils.cpp
    ../../../Common/LzFindPrepare.cpp
    ../../../Common/Md5Reg.cpp
    ../../../Common/MyMap.cpp
    ../../../Common/MyString.cpp
    ../../../Common/MyVector.cpp
    ../../../Common/MyWindows.cpp
    ../../../Common/MyXml.cpp
    ../../../Common/NewHandler.cpp
    ../../../Common/Random.cpp
    ../../../Common/Sha1Prepare.cpp
    ../../../Common/Sha1Reg.cpp
    ../../../Common/Sha256Prepare.cpp
    ../../../Common/Sha256Reg.cpp
    ../../../Common/Sha3Reg.cpp
    ../../../Common/Sha512Prepare.cpp
    ../../../Common/Sha512Reg.cpp
    ../../../Common/StdInStream.cpp
    ../../../Common/StdOutStream.cpp
    ../../../Common/StringConvert.cpp
    ../../../Common/StringToInt.cpp
    ../../../Common/TextConfig.cpp
    ../../../Common/UTFConvert.cpp
    ../../../Common/Wildcard.cpp
    ../../../Common/XzCrc64Init.cpp
    ../../../Common/XzCrc64Reg.cpp
    ../../../Common/Xxh64Reg.cpp
)

# Windows 特有源文件（仅在 WIN32 下使用）
set(WINDOWS_SOURCES
    ../../../Windows/Clipboard.cpp
    ../../../Windows/COM.cpp
    ../../../Windows/CommonDialog.cpp
    ../../../Windows/Console.cpp
    ../../../Windows/DLL.cpp
    ../../../Windows/MemoryGlobal.cpp
    ../../../Windows/MemoryLock.cpp
    ../../../Windows/Menu.cpp
    ../../../Windows/NationalTime.cpp
    ../../../Windows/Net.cpp
    ../../../Windows/ProcessMessages.cpp
    ../../../Windows/ProcessUtils.cpp
    ../../../Windows/Registry.cpp
    ../../../Windows/ResourceString.cpp
    ../../../Windows/SecurityUtils.cpp
    ../../../Windows/Shell.cpp
    ../../../Windows/Window.cpp
)

set(WINDOWS_CONTROL_SOURCES
    ../../../Windows/Control/ComboBox.cpp
    ../../../Windows/Control/Dialog.cpp
    ../../../Windows/Control/ImageList.cpp
    ../../../Windows/Control/ListView.cpp
    ../../../Windows/Control/PropertyPage.cpp
    ../../../Windows/Control/Window2.cpp
)

set(POSIX_ABSTRACTION_SOURCES
    ../../../Windows/FileIO.cpp
    ../../../Windows/FileDir.cpp
    ../../../Windows/FileFind.cpp
    ../../../Windows/FileName.cpp
    ../../../Windows/FileSystem.cpp
    ../../../Windows/FileLink.cpp
    ../../../Windows/TimeUtils.cpp
    ../../../Windows/PropVariant.cpp
    ../../../Windows/PropVariantConv.cpp
    ../../../Windows/ErrorMsg.cpp
    ../../../Windows/System.cpp
    ../../../Windows/SystemInfo.cpp
    ../../../Windows/Synchronization.cpp
    ../../../Windows/PropVariantUtils.cpp
)

set(COMMON_ARCHIVE_SOURCES
    ../../Common/CreateCoder.cpp
    ../../Common/CWrappers.cpp
    ../../Common/FilePathAutoRename.cpp
    ../../Common/FileStreams.cpp
    ../../Common/FilterCoder.cpp
    ../../Common/InBuffer.cpp
    ../../Common/InOutTempBuffer.cpp
    ../../Common/LimitedStreams.cpp
    ../../Common/LockedStream.cpp
    ../../Common/MemBlocks.cpp
    ../../Common/MethodId.cpp
    ../../Common/MethodProps.cpp
    ../../Common/MultiOutStream.cpp
    ../../Common/OffsetStream.cpp
    ../../Common/OutBuffer.cpp
    ../../Common/OutMemStream.cpp
    ../../Common/ProgressMt.cpp
    ../../Common/ProgressUtils.cpp
    ../../Common/PropId.cpp
    ../../Common/StreamBinder.cpp
    ../../Common/StreamObjects.cpp
    ../../Common/StreamUtils.cpp
    ../../Common/UniqBlocks.cpp
    ../../Common/VirtThread.cpp
)

set(ARCHIVE_SOURCES
    ../../Archive/ApfsHandler.cpp
    ../../Archive/ApmHandler.cpp
    ../../Archive/ArHandler.cpp
    ../../Archive/ArjHandler.cpp
    ../../Archive/AvbHandler.cpp
    ../../Archive/Base64Handler.cpp
    ../../Archive/Bz2Handler.cpp
    ../../Archive/ComHandler.cpp
    ../../Archive/CpioHandler.cpp
    ../../Archive/CramfsHandler.cpp
    ../../Archive/DeflateProps.cpp
    ../../Archive/DmgHandler.cpp
    ../../Archive/ElfHandler.cpp
    ../../Archive/ExtHandler.cpp
    ../../Archive/FatHandler.cpp
    ../../Archive/FlvHandler.cpp
    ../../Archive/GptHandler.cpp
    ../../Archive/GzHandler.cpp
    ../../Archive/HandlerCont.cpp
    ../../Archive/HfsHandler.cpp
    ../../Archive/IhexHandler.cpp
    ../../Archive/LpHandler.cpp
    ../../Archive/LvmHandler.cpp
    ../../Archive/LzhHandler.cpp
    ../../Archive/LzmaHandler.cpp
    ../../Archive/MachoHandler.cpp
    ../../Archive/MbrHandler.cpp
    ../../Archive/MslzHandler.cpp
    ../../Archive/MubHandler.cpp
    ../../Archive/NtfsHandler.cpp
    ../../Archive/PeHandler.cpp
    ../../Archive/PpmdHandler.cpp
    ../../Archive/QcowHandler.cpp
    ../../Archive/RpmHandler.cpp
    ../../Archive/SparseHandler.cpp
    ../../Archive/SplitHandler.cpp
    ../../Archive/SquashfsHandler.cpp
    ../../Archive/SwfHandler.cpp
    ../../Archive/UefiHandler.cpp
    ../../Archive/VdiHandler.cpp
    ../../Archive/VhdHandler.cpp
    ../../Archive/VhdxHandler.cpp
    ../../Archive/VmdkHandler.cpp
    ../../Archive/XarHandler.cpp
    ../../Archive/XzHandler.cpp
    ../../Archive/ZHandler.cpp
    ../../Archive/ZstdHandler.cpp
)

set(SEVEN_ZIP_SOURCES
    ../../Archive/7z/7zCompressionMode.cpp
    ../../Archive/7z/7zDecode.cpp
    ../../Archive/7z/7zEncode.cpp
    ../../Archive/7z/7zExtract.cpp
    ../../Archive/7z/7zFolderInStream.cpp
    ../../Archive/7z/7zHandler.cpp
    ../../Archive/7z/7zHandlerOut.cpp
    ../../Archive/7z/7zHeader.cpp
    ../../Archive/7z/7zIn.cpp
    ../../Archive/7z/7zOut.cpp
    ../../Archive/7z/7zProperties.cpp
    ../../Archive/7z/7zRegister.cpp
    ../../Archive/7z/7zSpecStream.cpp
    ../../Archive/7z/7zUpdate.cpp
)

set(CAB_SOURCES
    ../../Archive/Cab/CabBlockInStream.cpp
    ../../Archive/Cab/CabHandler.cpp
    ../../Archive/Cab/CabHeader.cpp
    ../../Archive/Cab/CabIn.cpp
    ../../Archive/Cab/CabRegister.cpp
)

set(CHM_SOURCES
    ../../Archive/Chm/ChmHandler.cpp
    ../../Archive/Chm/ChmIn.cpp
)

set(ISO_SOURCES
    ../../Archive/Iso/IsoHandler.cpp
    ../../Archive/Iso/IsoHeader.cpp
    ../../Archive/Iso/IsoIn.cpp
    ../../Archive/Iso/IsoRegister.cpp
)

set(NSIS_SOURCES
    ../../Archive/Nsis/NsisDecode.cpp
    ../../Archive/Nsis/NsisHandler.cpp
    ../../Archive/Nsis/NsisIn.cpp
    ../../Archive/Nsis/NsisRegister.cpp
)

set(RAR_SOURCES
    ../../Archive/Rar/Rar5Handler.cpp
    ../../Archive/Rar/RarHandler.cpp
)

set(TAR_SOURCES
    ../../Archive/Tar/TarHandler.cpp
    ../../Archive/Tar/TarHandlerOut.cpp
    ../../Archive/Tar/TarHeader.cpp
    ../../Archive/Tar/TarIn.cpp
    ../../Archive/Tar/TarOut.cpp
    ../../Archive/Tar/TarRegister.cpp
    ../../Archive/Tar/TarUpdate.cpp
)

set(UDF_SOURCES
    ../../Archive/Udf/UdfHandler.cpp
    ../../Archive/Udf/UdfIn.cpp
)

set(WIM_SOURCES
    ../../Archive/Wim/WimHandler.cpp
    ../../Archive/Wim/WimHandlerOut.cpp
    ../../Archive/Wim/WimIn.cpp
    ../../Archive/Wim/WimRegister.cpp
)

set(ZIP_SOURCES
    ../../Archive/Zip/ZipAddCommon.cpp
    ../../Archive/Zip/ZipHandler.cpp
    ../../Archive/Zip/ZipHandlerOut.cpp
    ../../Archive/Zip/ZipIn.cpp
    ../../Archive/Zip/ZipItem.cpp
    ../../Archive/Zip/ZipOut.cpp
    ../../Archive/Zip/ZipUpdate.cpp
    ../../Archive/Zip/ZipRegister.cpp
)

set(COMPRESS_SOURCES
    ../../Compress/Bcj2Coder.cpp
    ../../Compress/Bcj2Register.cpp
    ../../Compress/BcjCoder.cpp
    ../../Compress/BcjRegister.cpp
    ../../Compress/BitlDecoder.cpp
    ../../Compress/BranchMisc.cpp
    ../../Compress/BranchRegister.cpp
    ../../Compress/ByteSwap.cpp
    ../../Compress/BZip2Crc.cpp
    ../../Compress/BZip2Decoder.cpp
    ../../Compress/BZip2Encoder.cpp
    ../../Compress/BZip2Register.cpp
    ../../Compress/CopyCoder.cpp
    ../../Compress/CopyRegister.cpp
    ../../Compress/Deflate64Register.cpp
    ../../Compress/DeflateDecoder.cpp
    ../../Compress/DeflateEncoder.cpp
    ../../Compress/DeflateRegister.cpp
    ../../Compress/DeltaFilter.cpp
    ../../Compress/ImplodeDecoder.cpp
    ../../Compress/ImplodeHuffmanDecoder.cpp
    ../../Compress/LzfseDecoder.cpp
    ../../Compress/LzhDecoder.cpp
    ../../Compress/Lzma2Decoder.cpp
    ../../Compress/Lzma2Encoder.cpp
    ../../Compress/Lzma2Register.cpp
    ../../Compress/LzmaDecoder.cpp
    ../../Compress/LzmaEncoder.cpp
    ../../Compress/LzmaRegister.cpp
    ../../Compress/LzmsDecoder.cpp
    ../../Compress/LzOutWindow.cpp
    ../../Compress/LzxDecoder.cpp
    ../../Compress/PpmdDecoder.cpp
    ../../Compress/PpmdEncoder.cpp
    ../../Compress/PpmdRegister.cpp
    ../../Compress/PpmdZip.cpp
    ../../Compress/QuantumDecoder.cpp
    ../../Compress/Rar1Decoder.cpp
    ../../Compress/Rar2Decoder.cpp
    ../../Compress/Rar3Decoder.cpp
    ../../Compress/Rar3Vm.cpp
    ../../Compress/Rar5Decoder.cpp
    ../../Compress/RarCodecsRegister.cpp
    ../../Compress/ShrinkDecoder.cpp
    ../../Compress/XpressDecoder.cpp
    ../../Compress/XzDecoder.cpp
    ../../Compress/XzEncoder.cpp
    ../../Compress/ZDecoder.cpp
    ../../Compress/ZlibDecoder.cpp
    ../../Compress/ZlibEncoder.cpp
    ../../Compress/ZstdDecoder.cpp
)

set(CRYPTO_SOURCES
    ../../Crypto/7zAes.cpp
    ../../Crypto/7zAesRegister.cpp
    ../../Crypto/HmacSha1.cpp
    ../../Crypto/HmacSha256.cpp
    ../../Crypto/MyAes.cpp
    ../../Crypto/MyAesReg.cpp
    ../../Crypto/Pbkdf2HmacSha1.cpp
    ../../Crypto/RandGen.cpp
    ../../Crypto/Rar20Crypto.cpp
    ../../Crypto/Rar5Aes.cpp
    ../../Crypto/RarAes.cpp
    ../../Crypto/WzAes.cpp
    ../../Crypto/ZipCrypto.cpp
    ../../Crypto/ZipStrong.cpp
)

set(ARCHIVE_COMMON_SOURCES
    ../../Archive/Common/CoderMixer2.cpp
    ../../Archive/Common/DummyOutStream.cpp
    ../../Archive/Common/FindSignature.cpp
    ../../Archive/Common/HandlerOut.cpp
    ../../Archive/Common/InStreamWithCRC.cpp
    ../../Archive/Common/ItemNameUtils.cpp
    ../../Archive/Common/MultiStream.cpp
    ../../Archive/Common/OutStreamWithCRC.cpp
    ../../Archive/Common/OutStreamWithSha1.cpp
    ../../Archive/Common/ParseProperties.cpp
)

set(UI_COMMON_SOURCES
    ../../UI/Common/ArchiveCommandLine.cpp
    ../../UI/Common/ArchiveExtractCallback.cpp
    ../../UI/Common/ArchiveName.cpp
    ../../UI/Common/ArchiveOpenCallback.cpp
    ../../UI/Common/Bench.cpp
    ../../UI/Common/DefaultName.cpp
    ../../UI/Common/EnumDirItems.cpp
    ../../UI/Common/Extract.cpp
    ../../UI/Common/ExtractingFilePath.cpp
    ../../UI/Common/HashCalc.cpp
    ../../UI/Common/LoadCodecs.cpp
    ../../UI/Common/OpenArchive.cpp
    ../../UI/Common/PropIDUtils.cpp
    ../../UI/Common/SetProperties.cpp
    ../../UI/Common/SortUtils.cpp
    ../../UI/Common/TempFiles.cpp
    ../../UI/Common/Update.cpp
    ../../UI/Common/UpdateAction.cpp
    ../../UI/Common/UpdateCallback.cpp
    ../../UI/Common/UpdatePair.cpp
    ../../UI/Common/UpdateProduce.cpp
    ../../UI/Common/WorkDir.cpp
)

# Windows 特有的 UI 组件（全部排除，因为鸿蒙不需要）
set(UI_AGENT_SOURCES)
set(UI_CLIENT7Z_SOURCES)
set(UI_GUI_SOURCES)
set(UI_EXPLORER_SOURCES)
set(UI_FILEMANAGER_SOURCES)
set(SFX_CON_SOURCES)

# 控制台 UI（必须保留）
set(UI_CONSOLE_SOURCES
    ../../UI/Console/BenchCon.cpp
    ../../UI/Console/ConsoleClose.cpp
    ../../UI/Console/ExtractCallbackConsole.cpp
    ../../UI/Console/HashCon.cpp
    ../../UI/Console/List.cpp
    ../../UI/Console/Main.cpp
    ../../UI/Console/MainAr.cpp
    ../../UI/Console/OpenCallbackConsole.cpp
    ../../UI/Console/PercentPrinter.cpp
    ../../UI/Console/UpdateCallbackConsole.cpp
    ../../UI/Console/UserInputUtils.cpp
)

set(C_SOURCES
    ../../../../C/LzmaDec.c
    ../../../../C/7zAlloc.c
    ../../../../C/7zArcIn.c
    ../../../../C/7zBuf.c
    ../../../../C/7zBuf2.c
    ../../../../C/7zCrc.c
    ../../../../C/7zDec.c
    ../../../../C/7zFile.c
    ../../../../C/7zStream.c
    ../../../../C/Aes.c
    ../../../../C/Alloc.c
    ../../../../C/Bcj2.c
    ../../../../C/Bcj2Enc.c
    ../../../../C/Blake2s.c
    ../../../../C/Bra.c
    ../../../../C/Bra86.c
    ../../../../C/BraIA64.c
    ../../../../C/BwtSort.c
    ../../../../C/CpuArch.c
    ../../../../C/Delta.c
    ../../../../C/DllSecur.c
    ../../../../C/HuffEnc.c
    ../../../../C/LzFind.c
    ../../../../C/LzFindMt.c
    ../../../../C/Threads.c
    ../../../../C/LzmaEnc.c
    ../../../../C/Lzma86Dec.c
    ../../../../C/Lzma86Enc.c
    ../../../../C/Lzma2Dec.c
    ../../../../C/Lzma2DecMt.c
    ../../../../C/Lzma2Enc.c
    ../../../../C/LzmaLib.c
    ../../../../C/Md5.c
    ../../../../C/MtCoder.c
    ../../../../C/MtDec.c
    ../../../../C/Ppmd7.c
    ../../../../C/Ppmd7aDec.c
    ../../../../C/Ppmd7Dec.c
    ../../../../C/Ppmd7Enc.c
    ../../../../C/Ppmd8.c
    ../../../../C/Ppmd8Dec.c
    ../../../../C/Ppmd8Enc.c
    ../../../../C/Sha1.c
    ../../../../C/Sha256.c
    ../../../../C/Sha3.c
    ../../../../C/Sha512.c
    ../../../../C/Sha512Opt.c
    ../../../../C/SwapBytes.c
    ../../../../C/Xxh64.c
    ../../../../C/Xz.c
    ../../../../C/XzCrc64.c
    ../../../../C/XzDec.c
    ../../../../C/XzEnc.c
    ../../../../C/XzIn.c
    ../../../../C/ZstdDec.c
)

# 汇编文件（根据 USE_ASM 和架构选择）
if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
    set(IS_ARM64 ON)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8 AND CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|amd64")
    set(IS_X64 ON)
else()
    set(IS_X86 ON)
endif()

if(USE_ASM)
    if(IS_ARM64)
        # ARM64 汇编文件（GNU 语法）
        set(ASM_SOURCES
            ../../../../Asm/arm64/LzmaDecOpt.S
            ../../../../C/7zCrcOpt.c
            ../../../../C/XzCrc64Opt.c
            ../../../../C/Sha1Opt.c
            ../../../../C/Sha256Opt.c
            ../../../../C/Sort.c
            ../../../../C/AesOpt.c
            ../../../../C/LzFindOpt.c
        )
    elseif(IS_X64)
        # x86-64 汇编文件（MASM 语法）
        set(ASM_SOURCES
            ../../../../Asm/x86/7zCrcOpt.asm
            ../../../../Asm/x86/XzCrc64Opt.asm
            ../../../../Asm/x86/Sha1Opt.asm
            ../../../../Asm/x86/Sha256Opt.asm
            ../../../../Asm/x86/Sort.asm
            ../../../../Asm/x86/AesOpt.asm
            ../../../../Asm/x86/LzFindOpt.asm
            ../../../../Asm/x86/LzmaDecOpt.asm
        )
    else()
        # 32 位 x86 汇编
        set(ASM_SOURCES
            ../../../../Asm/x86/7zCrcOpt.asm
            ../../../../Asm/x86/XzCrc64Opt.asm
            ../../../../Asm/x86/Sha1Opt.asm
            ../../../../Asm/x86/Sha256Opt.asm
            ../../../../Asm/x86/Sort.asm
        )
    endif()
else()
    # 使用 C 优化文件（备选）
    set(ASM_SOURCES
        ../../../../C/7zCrcOpt.c
        ../../../../C/XzCrc64Opt.c
        ../../../../C/Sha1Opt.c
        ../../../../C/Sha256Opt.c
        ../../../../C/Sort.c
        ../../../../C/AesOpt.c
        ../../../../C/LzFindOpt.c
    )
endif()

# ================== 根据平台选择源文件 ==================
if(WIN32)
    set(PLATFORM_SPECIFIC_SOURCES
        ${WINDOWS_SOURCES}
        ${WINDOWS_CONTROL_SOURCES}
        ${UI_AGENT_SOURCES}
        ${UI_CLIENT7Z_SOURCES}
        ${UI_GUI_SOURCES}
        ${UI_EXPLORER_SOURCES}
        ${UI_FILEMANAGER_SOURCES}
        ${SFX_CON_SOURCES}
    )
else()
    set(PLATFORM_SPECIFIC_SOURCES
        ${UI_CONSOLE_SOURCES}
        ${POSIX_ABSTRACTION_SOURCES}
    )
    add_compile_definitions(__linux__)
endif()

# ================== 构建动态库 lib7za.so ==================
add_library(7za SHARED
    ${COMMON_SOURCES}
    ${COMMON_ARCHIVE_SOURCES}
    ${ARCHIVE_SOURCES}
    ${SEVEN_ZIP_SOURCES}
    ${CAB_SOURCES}
    ${CHM_SOURCES}
    ${ISO_SOURCES}
    ${NSIS_SOURCES}
    ${RAR_SOURCES}
    ${TAR_SOURCES}
    ${UDF_SOURCES}
    ${WIM_SOURCES}
    ${ZIP_SOURCES}
    ${COMPRESS_SOURCES}
    ${CRYPTO_SOURCES}
    ${ARCHIVE_COMMON_SOURCES}
    ${UI_COMMON_SOURCES}
    ${PLATFORM_SPECIFIC_SOURCES}
    ${C_SOURCES}
    ${ASM_SOURCES}
)


# 导出 main 符号
target_compile_definitions(7za PRIVATE EXPORT_MAIN JE_MALLOC)

# 链接库
if(WIN32)
    target_link_libraries(7za
        ole32 gdi32 comctl32 comdlg32 shell32 oleaut32 uuid advapi32 user32
    )
else()
    target_link_libraries(7za pthread dl)
endif()

# 设置动态库属性
set_target_properties(7za PROPERTIES
    OUTPUT_NAME "7za"
    VERSION "24.0"
    SOVERSION "1"
)

# 汇编优化
if(USE_ASM)
    target_compile_definitions(7za PRIVATE Z7_7ZIP_ASM)
endif()

target_link_options(7za PRIVATE
    -Wl,-Bsymbolic           # 优先解析库内符号，减少 PLT 跳转
    -Wl,-z,now               # 立即绑定所有符号（禁用懒绑定）
    -Wl,-z,relro             # 只读重定位段
    -Wl,--gc-sections        # 去除未使用的代码段
    -Wl,--icf=all            # 合并相同函数
)

# 架构定义（仅 Windows 需要）
if(WIN32)
    if(IS_X64)
        target_compile_definitions(7za PRIVATE _WIN64)
    else()
        target_compile_definitions(7za PRIVATE _WIN32)
    endif()
endif()

# ================== 复制动态库到 entry/libs 目录（供鸿蒙打包） ==================
if(NOT DEFINED OHOS_ARCH)
    set(OHOS_ARCH "arm64-v8a")
endif()
# 计算 entry 模块的根目录（假设当前文件位于 entry/src/main/cpp/7z2501-src/CPP/7zip/CMAKE/7za）
get_filename_component(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../.. ABSOLUTE)
set(LIBS_DIR ${PROJECT_ROOT}/entry/libs/${OHOS_ARCH})
file(MAKE_DIRECTORY ${LIBS_DIR})

add_custom_command(TARGET 7za POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:7za> ${LIBS_DIR}/lib7za.so
    COMMENT "Copying lib7za.so to ${LIBS_DIR}"
)