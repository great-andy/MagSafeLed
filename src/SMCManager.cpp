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

#include "SMCManager.h"

#include <IOKit/IOKitLib.h>
#include <format>

#pragma mark SMCBytes

SMCBytes::SMCBytes()
{
    std::memset(mData, 0, sizeof(mData));
}

size_t SMCBytes::SetHexString(const std::string& hexString)
{
    size_t numBytes = hexString.length();
    if (numBytes > 0)
    {
        if (numBytes == 1)
        {
            SInt32 val = SMCUtils::HexCharToInt(hexString[0]);
            if (val < 0)
            {
                numBytes = 0;
            }
            else
            {
                mData[0] = val;
            }
        }
        else
        {
            if ((numBytes > (sizeof(SMCBytes::mData) * 2)) || ((numBytes & 1) != 0))
            {
                numBytes = 0;
            }
            else
            {
                numBytes /= 2;
                for (size_t i = 0; i < numBytes; i++)
                {
                    SInt32 val1 = SMCUtils::HexCharToInt(hexString[i * 2]);
                    SInt32 val2 = SMCUtils::HexCharToInt(hexString[(i * 2) + 1]);
                    if ((val1 < 0) || (val2 < 0))
                    {
                        numBytes = 0;
                    }
                    else
                    {
                        mData[i] = (val1 << 4) + val2;
                    }
                }
            }
        }
    }
    return numBytes;
}

std::string SMCBytes::GetHexString(size_t numBytes, const std::string_view& separator) const
{
    std::string str;
    numBytes = std::min(numBytes, sizeof(mData));
    if (numBytes > 0)
    {
        size_t expectedSize = (numBytes * 2) + ((numBytes - 1) * separator.length());
        str.reserve(expectedSize);

        for (size_t i = 0; i < numBytes; i++)
        {
            if (i > 0)
            {
                str.append(separator);
            }
            std::format_to(std::back_inserter(str), "{:02x}", mData[i]);
        }
    }
    return str;
}

#pragma mark SMCValue

bool SMCValue::IsKeySizeValid() const
{
    return SMCUtils::IsKeySizeValid(mKey);
}

bool SMCValue::HasData() const
{
    return (mDataSize > 0);
}

size_t SMCValue::GetDataSize() const
{
    return mDataSize;
}

const SMCBytes& SMCValue::GetDataBytes() const
{
    return mBytes;
}

size_t SMCValue::SetDataSize(size_t size)
{
    mDataSize = std::min(size, sizeof(mBytes.mData));
    return mDataSize;
}

size_t SMCValue::SetHexString(const std::string& hexString)
{
    mDataSize = mBytes.SetHexString(hexString);
    return mDataSize;
}

size_t SMCValue::SetData(const SMCBytes& bytes, size_t size)
{
    SetDataSize(size);
    std::memcpy(mBytes.mData, bytes.mData, mDataSize);
    return mDataSize;
}

void SMCValue::SetDataBytes(const SMCBytes& dataBytes)
{
    mBytes = dataBytes;
}

UInt32 SMCValue::GetUInt32FromType() const
{
    UInt32 result = 0;
    if (mDataType == DATATYPE_UINT8)
        result = GetUInt8();
    else if (mDataType == DATATYPE_UINT16)
        result = GetUInt16();
    else if (mDataType == DATATYPE_UINT32)
        result = GetUInt32();
    return result;
}

SInt32 SMCValue::GetSInt32FromType() const
{
    UInt32 result = 0;
    if (mDataType == DATATYPE_SINT8)
        result = GetSInt8();
    else if (mDataType == DATATYPE_SINT16)
        result = GetSInt16();
    else if (mDataType == DATATYPE_SINT32)
        result = GetSInt32();
    return result;
}

UInt64 SMCValue::GetUInt64FromType() const
{
    UInt64 result = 0;
    if (mDataType == DATATYPE_UINT64)
        result = GetUInt64();
    else
        result = GetUInt32FromType();
    return result;
}

SInt64 SMCValue::GetSInt64FromType() const
{
    UInt64 result = 0;
    if (mDataType == DATATYPE_SINT64)
        result = GetSInt64();
    else
        result = GetSInt32FromType();
    return result;
}

float SMCValue::GetFloatFromType() const
{
    float result = 0;
    if (mDataType == DATATYPE_FLT)
        result = GetFloat();
    else if (mDataType == DATATYPE_FP1F)
        result = GetUInt16() / 32768.0;
    else if (mDataType == DATATYPE_FP4C)
        result = GetUInt16() / 4096.0;
    else if (mDataType == DATATYPE_FP5B)
        result = GetUInt16() / 2048.0;
    else if (mDataType == DATATYPE_FP6A)
        result = GetUInt16() / 1024.0;
    else if (mDataType == DATATYPE_FP79)
        result = GetUInt16() / 512.0;
    else if (mDataType == DATATYPE_FP88)
        result = GetUInt16() / 256.0;
    else if (mDataType == DATATYPE_FPA6)
        result = GetUInt16() / 64.0;
    else if (mDataType == DATATYPE_FPC4)
        result = GetUInt16() / 16.0;
    else if (mDataType == DATATYPE_FPE2)
        result = GetUInt16() / 4.0;
    else if (mDataType == DATATYPE_SP1E)
        result = GetSInt16() / 16384.0;
    else if (mDataType == DATATYPE_SP3C)
        result = GetSInt16() / 4096.0;
    else if (mDataType == DATATYPE_SP4B)
        result = GetSInt16() / 2048.0;
    else if (mDataType == DATATYPE_SP5A)
        result = GetSInt16() / 1024.0;
    else if (mDataType == DATATYPE_SP69)
        result = GetSInt16() / 512.0;
    else if (mDataType == DATATYPE_SP78)
        result = GetSInt16() / 256.0;
    else if (mDataType == DATATYPE_SP87)
        result = GetSInt16() / 128.0;
    else if (mDataType == DATATYPE_SP96)
        result = GetSInt16() / 64.0;
    else if (mDataType == DATATYPE_SPB4)
        result = GetSInt16() / 16.0;
    else if (mDataType == DATATYPE_SPF0)
        result = GetSInt16();
    else if (IsSignedInt())
        result = GetSInt64FromType();
    else if (IsUnsignedInt())
        result = GetUInt64FromType();
    return result;
}

double SMCValue::GetDoubleFromType() const
{
    double result = 0;
    if (mDataType == DATATYPE_IOFT)
        result = GetSInt64() / 65536.0;
    else if (IsSignedInt())
        result = GetSInt64FromType();
    else if (IsUnsignedInt())
        result = GetUInt64FromType();
    else
        result = GetFloatFromType();
    return result;
}

std::string SMCValue::GetString() const
{
    size_t numBytes = std::min(mDataSize, sizeof(mBytes.mData));
    return std::string(reinterpret_cast<const char*>(mBytes.mData), numBytes);
}

UInt8 SMCValue::GetUInt8() const
{
    UInt8 value = 0;
    if (mDataSize == sizeof(UInt8))
    {
        std::memcpy(&value, mBytes.mData, sizeof(value));
    }
    return value;
}

UInt16 SMCValue::GetUInt16() const
{
    UInt16 value = 0;
    if (mDataSize == sizeof(UInt16))
    {
        std::memcpy(&value, mBytes.mData, sizeof(value));
        if (IsMSB())
        {
            value = ntohs(value);
        }
    }
    return value;
}

UInt32 SMCValue::GetUInt32() const
{
    UInt32 value = 0;
    if (mDataSize == sizeof(UInt32))
    {
        std::memcpy(&value, mBytes.mData, sizeof(value));
        if (IsMSB())
        {
            value = ntohl(value);
        }
    }
    return value;
}

UInt64 SMCValue::GetUInt64() const
{
    UInt64 value = 0;
    if (mDataSize == sizeof(UInt64))
    {
        std::memcpy(&value, mBytes.mData, sizeof(value));
        if (IsMSB())
        {
            value = ntohll(value);
        }
    }
    return value;
}

SInt8 SMCValue::GetSInt8() const
{
    return std::bit_cast<SInt8>(GetUInt8());
}

SInt16 SMCValue::GetSInt16() const
{
    return std::bit_cast<SInt16>(GetUInt16());
}

SInt32 SMCValue::GetSInt32() const
{
    return std::bit_cast<SInt32>(GetUInt32());
}

SInt64 SMCValue::GetSInt64() const
{
    return std::bit_cast<SInt64>(GetUInt64());
}

float SMCValue::GetFloat() const
{
    float value = 0;
    if (mDataSize == sizeof(float))
    {
        std::memcpy(&value, mBytes.mData, sizeof(value));
    }
    return value;
}

bool SMCValue::IsMSB() const
{
    if (mDataAttributes & AttributeFlagLSB)
    {
        return false;
    }
    return true;
}

bool SMCValue::IsUnsignedInt() const
{
    return ((mDataType == DATATYPE_UINT8) ||
            (mDataType == DATATYPE_UINT16) ||
            (mDataType == DATATYPE_UINT32) ||
            (mDataType == DATATYPE_UINT64));
}

bool SMCValue::IsSignedInt() const
{
    return ((mDataType == DATATYPE_SINT8) ||
            (mDataType == DATATYPE_SINT16) ||
            (mDataType == DATATYPE_SINT32) ||
            (mDataType == DATATYPE_SINT64));
}

void SMCValue::PrintUInt32(UInt32 value) const
{
    printf("%u", value);
}

void SMCValue::PrintUInt64(UInt64 value) const
{
    printf("%llu", value);
}

void SMCValue::PrintSInt32(SInt32 value) const
{
    printf("%d", value);
}

void SMCValue::PrintSInt64(SInt64 value) const
{
    printf("%lld", value);
}

void SMCValue::PrintFloat(float value) const
{
    printf("%g", value);
}

void SMCValue::PrintDouble(double value) const
{
    printf("%lg", value);
}

void SMCValue::PrintFlag() const
{
    printf("%s", (GetUInt8() != 0) ? "true" : "false");
}

void SMCValue::PrintCh8p() const
{
    std::string str = GetString();

    // trim \0 from right
    if (!str.empty())
    {
        str.erase(str.find_last_not_of('\0') + 1);
    }

    printf("\"");
    for (unsigned char c : str)
    {
        printf("%c", (c < 32) ? '.' : c);
    }
    printf("\"");
}

void SMCValue::PrintPWM() const
{
    printf("%g%%", GetUInt16() * 100.0 / 65536.0);
}

void SMCValue::PrintBytesHex() const
{
    printf("  (");
    printf("%s", mBytes.GetHexString(mDataSize, " ").c_str());
    printf(")\n");
}

void SMCValue::Print() const
{
    printf("   %-4s  [%-4s|%02x]  ", mKey.c_str(), mDataType.c_str(), mDataAttributes);
    if (HasData())
    {
        if ((mDataType == DATATYPE_UINT8) ||
            (mDataType == DATATYPE_UINT16) ||
            (mDataType == DATATYPE_UINT32))
            PrintUInt32(GetUInt32FromType());
        else if (mDataType == DATATYPE_UINT64)
            PrintUInt64(GetUInt64());
        else if ((mDataType == DATATYPE_SINT8) ||
                 (mDataType == DATATYPE_SINT16) ||
                 (mDataType == DATATYPE_SINT32))
            PrintSInt32(GetSInt32FromType());
        else if (mDataType == DATATYPE_SINT64)
            PrintSInt64(GetSInt64());
        else if ((mDataType == DATATYPE_FLT) ||
                 (mDataType == DATATYPE_FP1F) ||
                 (mDataType == DATATYPE_FP4C) ||
                 (mDataType == DATATYPE_FP5B) ||
                 (mDataType == DATATYPE_FP6A) ||
                 (mDataType == DATATYPE_FP79) ||
                 (mDataType == DATATYPE_FP88) ||
                 (mDataType == DATATYPE_FPA6) ||
                 (mDataType == DATATYPE_FPC4) ||
                 (mDataType == DATATYPE_FPE2) ||
                 (mDataType == DATATYPE_SP1E) ||
                 (mDataType == DATATYPE_SP3C) ||
                 (mDataType == DATATYPE_SP4B) ||
                 (mDataType == DATATYPE_SP5A) ||
                 (mDataType == DATATYPE_SP69) ||
                 (mDataType == DATATYPE_SP78) ||
                 (mDataType == DATATYPE_SP87) ||
                 (mDataType == DATATYPE_SP96) ||
                 (mDataType == DATATYPE_SPB4) ||
                 (mDataType == DATATYPE_SPF0))
            PrintFloat(GetFloatFromType());
        else if (mDataType == DATATYPE_IOFT)
            PrintDouble(GetDoubleFromType());
        else if (mDataType == DATATYPE_FLAG)
            PrintFlag();
        else if (mDataType == DATATYPE_CH8P)
            PrintCh8p();
        else if (mDataType == DATATYPE_PWM)
            PrintPWM();
        else if (mDataType == DATATYPE_HEX)
            printf("#%zu", mDataSize);
        else
            printf("? #%zu", mDataSize);

        PrintBytesHex();
    }
    else
    {
        printf("no data\n");
    }
}

#pragma mark SMCUtils

UInt32 SMCUtils::StringToUInt32(const std::string& str)
{
    UInt32 value = 0;
    size_t size = std::min(str.length(), sizeof(value));
    for (size_t i = 0; i < size; i++)
    {
        value += (UInt32(static_cast<unsigned char>(str[i])) << ((size - i - 1) * 8));
    }
    return value;
}

std::string SMCUtils::UInt32ToString(UInt32 value)
{
    std::string str(sizeof(value), 0);
    for (size_t i = 0; i < sizeof(value); i++)
    {
        str[i] = value >> ((sizeof(value) - i - 1) * 8);
    }
    return str;
}

SInt32 SMCUtils::HexCharToInt(char c)
{
    SInt32 value = -1;
    if ((c >= '0') && (c <= '9'))
    {
        value =  c - '0';
    }
    else if ((c >= 'a') && (c <= 'f'))
    {
        value = c - 'a' + 10;
    }
    else if ((c >= 'A') && (c <= 'F'))
    {
        value = c - 'A' + 10;
    }
    return value;
}

bool SMCUtils::IsKeySizeValid(const std::string& key)
{
    return (key.length() == sizeof(SMCKeyData::mKey));
}

#pragma mark SMCTool

SMCManager::~SMCManager()
{
    Close();
}

bool SMCManager::Open()
{
    if (mConn == 0)
    {
        io_service_t service = IOServiceGetMatchingService(kIOMainPortDefault, IOServiceMatching(std::string(SMCServiceName).c_str()));
        if (service == IO_OBJECT_NULL)
        {
            printf("Error: SMCManager IOServiceGetMatchingServices() failed\n");
        }
        else
        {
            kern_return_t result = IOServiceOpen(service, mach_task_self(), 0, &mConn);
            IOObjectRelease(service);
            if (result != KERN_SUCCESS)
            {
                printf("Error: SMCManager IOServiceOpen() = %08x\n", result);
            }
        }
    }
    else
    {
        printf("Info: SMCManager is already open\n");
    }
    return (mConn != 0);
}

void SMCManager::Close()
{
    if (mConn != 0)
    {
        IOServiceClose(mConn);
        mConn = 0;
    }
}

bool SMCManager::IsOpen() const
{
    return (mConn != 0);
}

bool SMCManager::ReadKey(const std::string& key, SMCValue& returnValue)
{
    bool success = false;
    if (mConn != 0)
    {
        if (!SMCUtils::IsKeySizeValid(key))
        {
            printf("Error: SMCManager ReadKey(\"%s\") inavlid key\n", key.c_str());
        }
        else
        {
            kern_return_t result = ReadKeyRaw(key, returnValue);
            if (result != kIOReturnSuccess)
            {
                printf("Error: SMCManager ReadKey(\"%s\") = %08x\n", key.c_str(), result);
            }
            else
            {
                success = true;
            }
        }
    }
    else
    {
        printf("Error: SMCManager not ready, initialize with Open()\n");
    }
    return success;
}

bool SMCManager::WriteKey(const std::string& key, UInt8 value)
{
    SMCBytes data;
    data.mData[0] = value;
    SMCValue writeValue;
    writeValue.mKey = key;
    writeValue.SetData(data, sizeof(value));
    return WriteValue(writeValue);
}

bool SMCManager::WriteKey(const std::string& key, const std::string& hexString)
{
    SMCValue writeValue;
    if (writeValue.SetHexString(hexString) > 0)
    {
        writeValue.mKey = key;
        return WriteValue(writeValue);
    }
    else
    {
        printf("Error: SMCManager invalid hex string\n");
    }
    return false;
}

bool SMCManager::WriteValue(const SMCValue& writeValue)
{
    bool success = false;
    if (mConn != 0)
    {
        if (!writeValue.IsKeySizeValid())
        {
            printf("Error: SMCManager WriteValue() inavlid key \"%s\"\n", writeValue.mKey.c_str());
        }
        else
        {
            kern_return_t result = WriteValueRaw(writeValue);
            if (result != kIOReturnSuccess)
            {
                printf("Error: SMCManager WriteValue() = %08x\n", result);
            }
            else
            {
                success = true;
            }
        }
    }
    else
    {
        printf("Error: SMCManager not ready, initialize with Open()\n");
    }
    return success;
}

kern_return_t SMCManager::ReadKeyRaw(const std::string& key, SMCValue& returnValue)
{
    returnValue.mKey = key;
    SMCKeyData keyData;
    keyData.mKey = SMCUtils::StringToUInt32(key);
    SMCKeyData returnData;
    kern_return_t result = GetKeyInfo(keyData.mKey, returnData.mKeyInfo);
    if (result == kIOReturnSuccess)
    {
        returnValue.mDataType = SMCUtils::UInt32ToString(returnData.mKeyInfo.mDataType);
        returnValue.mDataAttributes = returnData.mKeyInfo.mDataAttributes;
        returnValue.SetDataSize(returnData.mKeyInfo.mDataSize);

        keyData.mKeyInfo.mDataSize = SInt32(returnValue.GetDataSize());
        keyData.mCommand = SMC_CMD_READ_BYTES;
        result = IOConnectCall(keyData, returnData);
        if (result == kIOReturnSuccess)
        {
            returnValue.SetDataBytes(returnData.mBytes);
        }
    }
    else
    {
        returnValue.mDataType.clear();
        returnValue.mDataAttributes = 0;
        returnValue.SetDataSize(0);
    }
    return result;
}

kern_return_t SMCManager::GetKeyInfo(UInt32 key, SMCKeyDataKeyInfo& keyInfo)
{
    kern_return_t result = kIOReturnSuccess;

    KeyInfoMapT::iterator iter = mCacheItems.find(key);
    if (iter != mCacheItems.end())
    {
        keyInfo = iter->second;
    }
    else
    {
        SMCKeyData keyData;
        keyData.mKey = key;
        keyData.mCommand = SMC_CMD_READ_KEYINFO;

        SMCKeyData returnData;
        result = IOConnectCall(keyData, returnData);
        if (result == kIOReturnSuccess)
        {
            keyInfo = returnData.mKeyInfo;
            mCacheItems[key] = keyInfo;
        }
    }
    return result;
}

kern_return_t SMCManager::WriteValueRaw(const SMCValue& writeVal)
{
    SMCValue readVal;
    kern_return_t result = ReadKeyRaw(writeVal.mKey, readVal);
    if (result == kIOReturnSuccess)
    {
        if (readVal.GetDataSize() != writeVal.GetDataSize())
        {
            result = kIOReturnError;
        }
        else
        {
            SMCKeyData keyData;
            keyData.mKey = SMCUtils::StringToUInt32(writeVal.mKey);
            keyData.mCommand = SMC_CMD_WRITE_BYTES;
            keyData.mKeyInfo.mDataSize = UInt32(writeVal.GetDataSize());
            keyData.mBytes = writeVal.GetDataBytes();

            SMCKeyData returnData;
            result = IOConnectCall(keyData, returnData);
        }
    }
    return result;
}

kern_return_t SMCManager::IOConnectCall(const SMCKeyData& keyData, SMCKeyData& returnData)
{
    size_t returnDataSize = sizeof(returnData);
    return IOConnectCallStructMethod(mConn, KERNEL_INDEX_SMC, &keyData, sizeof(keyData), &returnData, &returnDataSize);
}
