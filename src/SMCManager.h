/*
 * Apple System Management Control (SMC) Manager
 * Copyright (C) 2006 devnull
 * Portions Copyright (C) 2013 Michael Wilber
 * C++ Version Copyright (C) 2026 Great Andy
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <IOKit/IOTypes.h>
#include <string>
#include <unordered_map>

struct SMCKeyDataVersion
{
    UInt8   mMajor = 0;
    UInt8   mMinor = 0;
    UInt8   mBuild = 0;
    UInt8   mReserved = 0;
    UInt16  mRelease = 0;
    UInt8   _padding[2] = {0, 0};
};

struct SMCKeyDataLimit
{
    UInt16  mVersion = 0;
    UInt16  mLength = 0;
    UInt32  mCpuPLimit = 0;
    UInt32  mGpuPLimit = 0;
    UInt32  mMemPLimit = 0;
};

struct SMCKeyDataKeyInfo
{
    UInt32  mDataSize = 0;
    UInt32  mDataType = 0;
    UInt8   mDataAttributes = 0;
    UInt8   _padding[3] = {0, 0, 0};
};

class SMCBytes
{
public:
    SMCBytes();

    size_t SetHexString(const std::string& hexString);
    std::string GetHexString(size_t numBytes, const std::string_view& separator = "") const;

    UInt8 mData[32];
};

struct SMCKeyData
{
    UInt32              mKey = 0;
    SMCKeyDataVersion   mVersion;
    SMCKeyDataLimit     mLimit;
    SMCKeyDataKeyInfo   mKeyInfo;
    UInt8               mResult = 0;
    UInt8               mStatus = 0;
    UInt8               mCommand = 0;
    UInt8               _padding = 0;
    UInt32              mData32 = 0;
    SMCBytes            mBytes;
};

static_assert(sizeof(SMCKeyData) == 80, "SMCKeyData has wrong size.");

class SMCValue
{
public:
    std::string mKey;
    std::string mDataType;
    UInt8       mDataAttributes = 0;

protected:
    size_t      mDataSize = 0;
    SMCBytes    mBytes;

public:
    bool IsKeySizeValid() const;
    bool HasData() const;
    size_t GetDataSize() const;
    size_t SetDataSize(size_t size);
    const SMCBytes& GetDataBytes() const;

    size_t SetHexString(const std::string& hexString);
    size_t SetData(const SMCBytes& data, size_t size);
    void SetDataBytes(const SMCBytes& dataBytes);

    UInt32 GetUInt32FromType() const;
    SInt32 GetSInt32FromType() const;
    UInt64 GetUInt64FromType() const;
    SInt64 GetSInt64FromType() const;

    float GetFloatFromType() const;
    double GetDoubleFromType() const;

    std::string GetString() const;
    UInt8 GetUInt8() const;
    UInt16 GetUInt16() const;
    UInt32 GetUInt32() const;
    UInt64 GetUInt64() const;
    SInt8 GetSInt8() const;
    SInt16 GetSInt16() const;
    SInt32 GetSInt32() const;
    SInt64 GetSInt64() const;
    float GetFloat() const;

    bool IsMSB() const;
    bool IsUnsignedInt() const;
    bool IsSignedInt() const;

    static constexpr UInt8 AttributeFlagLSB = 0x04;

    static constexpr std::string_view DATATYPE_UINT8  = "ui8 ";
    static constexpr std::string_view DATATYPE_UINT16 = "ui16";
    static constexpr std::string_view DATATYPE_UINT32 = "ui32";
    static constexpr std::string_view DATATYPE_UINT64 = "ui64";
    static constexpr std::string_view DATATYPE_SINT8  = "si8 ";
    static constexpr std::string_view DATATYPE_SINT16 = "si16";
    static constexpr std::string_view DATATYPE_SINT32 = "si32";
    static constexpr std::string_view DATATYPE_SINT64 = "si64";

    static constexpr std::string_view DATATYPE_FLT  = "flt ";
    static constexpr std::string_view DATATYPE_FP1F = "fp1f";
    static constexpr std::string_view DATATYPE_FP4C = "fp4c";
    static constexpr std::string_view DATATYPE_FP5B = "fp5b";
    static constexpr std::string_view DATATYPE_FP6A = "fp6a";
    static constexpr std::string_view DATATYPE_FP79 = "fp79";
    static constexpr std::string_view DATATYPE_FP88 = "fp88";
    static constexpr std::string_view DATATYPE_FPA6 = "fpa6";
    static constexpr std::string_view DATATYPE_FPC4 = "fpc4";
    static constexpr std::string_view DATATYPE_FPE2 = "fpe2";

    static constexpr std::string_view DATATYPE_SP1E = "sp1e";
    static constexpr std::string_view DATATYPE_SP3C = "sp3c";
    static constexpr std::string_view DATATYPE_SP4B = "sp4b";
    static constexpr std::string_view DATATYPE_SP5A = "sp5a";
    static constexpr std::string_view DATATYPE_SP69 = "sp69";
    static constexpr std::string_view DATATYPE_SP78 = "sp78";
    static constexpr std::string_view DATATYPE_SP87 = "sp87";
    static constexpr std::string_view DATATYPE_SP96 = "sp96";
    static constexpr std::string_view DATATYPE_SPB4 = "spb4";
    static constexpr std::string_view DATATYPE_SPF0 = "spf0";

    static constexpr std::string_view DATATYPE_IOFT = "ioft";

    static constexpr std::string_view DATATYPE_FLAG = "flag";
    static constexpr std::string_view DATATYPE_CH8P = "ch8*";
    static constexpr std::string_view DATATYPE_PWM  = "{pwm";
    static constexpr std::string_view DATATYPE_HEX  = "hex_";

    void PrintUInt32(UInt32 value) const;
    void PrintUInt64(UInt64 value) const;
    void PrintSInt32(SInt32 value) const;
    void PrintSInt64(SInt64 value) const;

    void PrintFloat(float value) const;
    void PrintDouble(double value) const;

    void PrintFlag() const;
    void PrintCh8p() const;
    void PrintPWM() const;

    void PrintBytesHex() const;

    void Print() const;
};

class SMCUtils
{
public:
    static UInt32 StringToUInt32(const std::string& str);
    static std::string UInt32ToString(UInt32 value);

    static SInt32 HexCharToInt(char c);
    static bool IsKeySizeValid(const std::string& key);
};

class SMCManager
{
public:
    virtual ~SMCManager();

    SMCManager() = default;
    SMCManager(const SMCManager&) = delete;
    SMCManager& operator=(const SMCManager&) = delete;
    SMCManager(SMCManager&&) = delete;
    SMCManager& operator=(SMCManager&&) = delete;

    static constexpr std::string_view SMCServiceName  = "AppleSMC";

    enum SMCSelector
    {
        KERNEL_INDEX_SMC = 2
    };

    enum SMCCommand
    {
        SMC_CMD_READ_BYTES   = 5,
        SMC_CMD_WRITE_BYTES  = 6,
        SMC_CMD_READ_INDEX   = 8,
        SMC_CMD_READ_KEYINFO = 9,
        SMC_CMD_READ_PLIMIT  = 11,
        SMC_CMD_READ_VERSION = 12
    };

    bool Open();
    void Close();
    bool IsOpen() const;

    bool ReadKey(const std::string& key, SMCValue& returnValue);
    bool WriteKey(const std::string& key, UInt8 value);
    bool WriteKey(const std::string& key, const std::string& hexString);
    bool WriteValue(const SMCValue& writeValue);

    kern_return_t ReadKeyRaw(const std::string& key, SMCValue& returnValue);

    kern_return_t GetKeyInfo(UInt32 key, SMCKeyDataKeyInfo& keyInfo);

    kern_return_t WriteValueRaw(const SMCValue& writeVal);

    kern_return_t IOConnectCall(const SMCKeyData& keyData, SMCKeyData& returnData);

protected:
    io_connect_t mConn = 0;

    typedef std::unordered_map<UInt32, SMCKeyDataKeyInfo> KeyInfoMapT;
    KeyInfoMapT mCacheItems;
};
