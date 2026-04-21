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
            numBytes = std::min(numBytes / 2, sizeof(mData));
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
    return numBytes;
}

void SMCBytes::PrintBytesHex(size_t numBytes) const
{
    printf("(bytes");
    numBytes = std::min(numBytes, sizeof(mData));
    for (size_t i = 0; i < numBytes; i++)
    {
        printf(" %02x", UInt8(mData[i]));
    }
    printf(")\n");
}

#pragma mark SMCValue

void SMCValue::PrintUInt() const
{
    printf("%u ", SMCUtils::GetUInt32(std::string(reinterpret_cast<const char*>(mBytes.mData), mDataSize)));
}

void SMCValue::PrintFLT() const
{
    if (mDataSize == sizeof(float))
    {
        printf("%.0f ", *reinterpret_cast<const float*>(mBytes.mData));
    }
}

void SMCValue::PrintFP1F() const
{
    if (mDataSize == sizeof(UInt16))
    {
        printf("%.5f ", ntohs(*reinterpret_cast<const UInt16*>(mBytes.mData)) / 32768.0);
    }
}

void SMCValue::PrintFP4C() const
{
    if (mDataSize == sizeof(UInt16))
    {
        printf("%.5f ", ntohs(*reinterpret_cast<const UInt16*>(mBytes.mData)) / 4096.0);
    }
}

void SMCValue::PrintFP5B() const
{
    if (mDataSize == sizeof(UInt16))
    {
        printf("%.5f ", ntohs(*reinterpret_cast<const UInt16*>(mBytes.mData)) / 2048.0);
    }
}

void SMCValue::PrintFP6A() const
{
    if (mDataSize == sizeof(UInt16))
    {
        printf("%.4f ", ntohs(*reinterpret_cast<const UInt16*>(mBytes.mData)) / 1024.0);
    }
}

void SMCValue::PrintFP79() const
{
    if (mDataSize == sizeof(UInt16))
    {
        printf("%.4f ", ntohs(*reinterpret_cast<const UInt16*>(mBytes.mData)) / 512.0);
    }
}

void SMCValue::PrintFP88() const
{
    if (mDataSize == sizeof(UInt16))
    {
        printf("%.3f ", ntohs(*reinterpret_cast<const UInt16*>(mBytes.mData)) / 256.0);
    }
}

void SMCValue::PrintFPA6() const
{
    if (mDataSize == sizeof(UInt16))
    {
        printf("%.2f ", ntohs(*reinterpret_cast<const UInt16*>(mBytes.mData)) / 64.0);
    }
}

void SMCValue::PrintFPC4() const
{
    if (mDataSize == sizeof(UInt16))
    {
        printf("%.2f ", ntohs(*reinterpret_cast<const UInt16*>(mBytes.mData)) / 16.0);
    }
}

void SMCValue::PrintFPE2() const
{
    if (mDataSize == sizeof(UInt16))
    {
        printf("%.2f ", ntohs(*reinterpret_cast<const UInt16*>(mBytes.mData)) / 4.0);
    }
}

void SMCValue::PrintSP1E() const
{
    if (mDataSize == sizeof(UInt16))
    {
        printf("%.5f ", SInt16(ntohs(*reinterpret_cast<const UInt16*>(mBytes.mData))) / 16384.0);
    }
}

void SMCValue::PrintSP3C() const
{
    if (mDataSize == sizeof(UInt16))
    {
        printf("%.5f ", SInt16(ntohs(*reinterpret_cast<const UInt16*>(mBytes.mData))) / 4096.0);
    }
}

void SMCValue::PrintSP4B() const
{
    if (mDataSize == sizeof(UInt16))
    {
        printf("%.4f ", SInt16(ntohs(*reinterpret_cast<const UInt16*>(mBytes.mData))) / 2048.0);
    }
}

void SMCValue::PrintSP5A() const
{
    if (mDataSize == sizeof(UInt16))
    {
        printf("%.4f ", SInt16(ntohs(*reinterpret_cast<const UInt16*>(mBytes.mData))) / 1024.0);
    }
}

void SMCValue::PrintSP69() const
{
    if (mDataSize == sizeof(UInt16))
    {
        printf("%.3f ", SInt16(ntohs(*reinterpret_cast<const UInt16*>(mBytes.mData))) / 512.0);
    }
}

void SMCValue::PrintSP78() const
{
    if (mDataSize == sizeof(UInt16))
    {
        printf("%.3f ", SInt16(ntohs(*reinterpret_cast<const UInt16*>(mBytes.mData))) / 256.0);
    }
}

void SMCValue::PrintSP87() const
{
    if (mDataSize == sizeof(UInt16))
    {
        printf("%.3f ", SInt16(ntohs(*reinterpret_cast<const UInt16*>(mBytes.mData))) / 128.0);
    }
}

void SMCValue::PrintSP96() const
{
    if (mDataSize == sizeof(UInt16))
    {
        printf("%.2f ", SInt16(ntohs(*reinterpret_cast<const UInt16*>(mBytes.mData))) / 64.0);
    }
}

void SMCValue::PrintSPB4() const
{
    if (mDataSize == sizeof(UInt16))
    {
        printf("%.2f ", SInt16(ntohs(*reinterpret_cast<const UInt16*>(mBytes.mData))) / 16.0);
    }
}

void SMCValue::PrintSPF0() const
{
    if (mDataSize == sizeof(UInt16))
    {
        printf("%.0f ", float(ntohs(*reinterpret_cast<const UInt16*>(mBytes.mData))));
    }
}

void SMCValue::PrintSI8() const
{
    if (mDataSize == sizeof(SInt8))
    {
        printf("%d ", SInt8(*mBytes.mData));
    }
}

void SMCValue::PrintSI16() const
{
    if (mDataSize == sizeof(SInt16))
    {
        printf("%d ", ntohs(*reinterpret_cast<const SInt16*>(mBytes.mData)));
    }
}

void SMCValue::PrintPWM() const
{
    if (mDataSize == sizeof(UInt16))
    {
        printf("%.1f%% ", ntohs(*reinterpret_cast<const UInt16*>(mBytes.mData)) * 100 / 65536.0);
    }
}

void SMCValue::PrintBytesHex() const
{
    mBytes.PrintBytesHex(mDataSize);
}

void SMCValue::Print() const
{
    printf("  %-4s  [%-4s]  ", mKey.c_str(), mDataType.c_str());
    if (mDataSize > 0)
    {
        if ((mDataType == DATATYPE_UINT8) || (mDataType == DATATYPE_UINT16) || (mDataType == DATATYPE_UINT32))
            PrintUInt();
        else if (mDataType == DATATYPE_FLT)
            PrintFLT();
        else if (mDataType == DATATYPE_FP1F)
            PrintFP1F();
        else if (mDataType == DATATYPE_FP4C)
            PrintFP4C();
        else if (mDataType == DATATYPE_FP5B)
            PrintFP5B();
        else if (mDataType == DATATYPE_FP6A)
            PrintFP6A();
        else if (mDataType == DATATYPE_FP79)
            PrintFP79();
        else if (mDataType == DATATYPE_FP88)
            PrintFP88();
        else if (mDataType == DATATYPE_FPA6)
            PrintFPA6();
        else if (mDataType == DATATYPE_FPC4)
            PrintFPC4();
        else if (mDataType == DATATYPE_FPE2)
            PrintFPE2();
        else if (mDataType == DATATYPE_SP1E)
            PrintSP1E();
        else if (mDataType == DATATYPE_SP3C)
            PrintSP3C();
        else if (mDataType == DATATYPE_SP4B)
            PrintSP4B();
        else if (mDataType == DATATYPE_SP5A)
            PrintSP5A();
        else if (mDataType == DATATYPE_SP69)
            PrintSP69();
        else if (mDataType == DATATYPE_SP78)
            PrintSP78();
        else if (mDataType == DATATYPE_SP87)
            PrintSP87();
        else if (mDataType == DATATYPE_SP96)
            PrintSP96();
        else if (mDataType == DATATYPE_SPB4)
            PrintSPB4();
        else if (mDataType == DATATYPE_SPF0)
            PrintSPF0();
        else if (mDataType == DATATYPE_SI8)
            PrintSI8();
        else if (mDataType == DATATYPE_SI16)
            PrintSI16();
        else if (mDataType == DATATYPE_PWM)
            PrintPWM();

        PrintBytesHex();
    }
    else
    {
        printf("no data\n");
    }
}

#pragma mark SMCUtils

UInt32 SMCUtils::GetUInt32(const std::string& str)
{
    UInt32 value = 0;
    size_t size = std::min(str.length(), sizeof(value));
    for (size_t i = 0; i < size; i++)
    {
        value += (UInt32(str[i]) << ((size - i - 1) * 8));
    }
    return value;
}

std::string SMCUtils::GetString(UInt32 value)
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

#pragma mark SMCTool

SMCManager::~SMCManager()
{
    Close();
}

bool SMCManager::Open()
{
    if (mConn == 0)
    {
        io_service_t service = IOServiceGetMatchingService(kIOMainPortDefault, IOServiceMatching("AppleSMC"));
        if (service == IO_OBJECT_NULL)
        {
            printf("Error: IOServiceGetMatchingServices() failed\n");
        }
        else
        {
            kern_return_t result = IOServiceOpen(service, mach_task_self(), 0, &mConn);
            IOObjectRelease(service);
            if (result != KERN_SUCCESS)
            {
                printf("Error: IOServiceOpen() = %08x\n", result);
            }
        }
    }
    else
    {
        printf("Info: AppleSMC is already open");
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

bool SMCManager::WriteKey(const std::string& key, UInt8 value)
{
    bool ret = false;
    if (mConn != 0)
    {
        SMCValue writeValue;
        writeValue.mKey = key;
        writeValue.mBytes.mData[0] = value;
        writeValue.mDataSize = sizeof(value);

        kern_return_t result = WriteValue(writeValue);
        if (result != kIOReturnSuccess)
        {
            printf("Error: SMCWriteKey() = %08x\n", result);
        }
        else
        {
            ret = true;
        }
    }
    else
    {
        printf("Error: AppleSMC not ready, initialize with Open()");
    }
    return ret;
}

bool SMCManager::WriteKey(const std::string& key, const std::string& hexString)
{
    bool ret = false;
    if (mConn != 0)
    {
        SMCValue writeValue;
        size_t numBytes = writeValue.mBytes.SetHexString(hexString);
        if (numBytes > 0)
        {
            writeValue.mKey = key;
            writeValue.mDataSize = UInt32(numBytes);

            kern_return_t result = WriteValue(writeValue);
            if (result != kIOReturnSuccess)
            {
                printf("Error: SMCWriteKey() = %08x\n", result);
            }
            else
            {
                ret = true;
            }
        }
    }
    else
    {
        printf("Error: AppleSMC not ready, initialize with Open()");
    }
    return ret;
}

kern_return_t SMCManager::ReadKey(const std::string& key, SMCValue& returnValue)
{
    SMCKeyData keyData;
    keyData.mKey = SMCUtils::GetUInt32(key);
    SMCKeyData returnData;
    kern_return_t result = GetKeyInfo(keyData.mKey, returnData.mKeyInfo);
    if (result == kIOReturnSuccess)
    {
        returnValue.mKey = key;
        returnValue.mDataSize = returnData.mKeyInfo.mDataSize;
        returnValue.mDataType = SMCUtils::GetString(returnData.mKeyInfo.mDataType);
        keyData.mKeyInfo.mDataSize = returnValue.mDataSize;
        keyData.mCommand = SMC_CMD_READ_BYTES;

        result = IOConnectCall(KERNEL_INDEX_SMC, keyData, returnData);
        if (result == kIOReturnSuccess)
        {
            returnValue.mBytes = returnData.mBytes;
        }
    }
    return result;
}

kern_return_t SMCManager::GetKeyInfo(UInt32 key, SMCKeyDataKeyInfo& keyInfo)
{
    kern_return_t result = kIOReturnSuccess;
    os_unfair_lock_lock(&mLock);

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
        result = IOConnectCall(KERNEL_INDEX_SMC, keyData, returnData);
        if (result == kIOReturnSuccess)
        {
            keyInfo = returnData.mKeyInfo;
            mCacheItems[key] = keyInfo;
        }
    }

    os_unfair_lock_unlock(&mLock);
    return result;
}

kern_return_t SMCManager::WriteValue(const SMCValue& writeVal)
{
    SMCValue readVal;
    kern_return_t result = ReadKey(writeVal.mKey, readVal);
    if (result == kIOReturnSuccess)
    {
        if (readVal.mDataSize != writeVal.mDataSize)
        {
            result = kIOReturnError;
        }
        else
        {
            SMCKeyData keyData;
            keyData.mKey = SMCUtils::GetUInt32(writeVal.mKey);
            keyData.mCommand = SMC_CMD_WRITE_BYTES;
            keyData.mKeyInfo.mDataSize = writeVal.mDataSize;
            keyData.mBytes = writeVal.mBytes;

            SMCKeyData returnData;
            result = IOConnectCall(KERNEL_INDEX_SMC, keyData, returnData);
        }
    }
    return result;
}

kern_return_t SMCManager::IOConnectCall(SMCSelector index, const SMCKeyData& keyData, SMCKeyData& returnData)
{
    size_t returnDataSize = sizeof(returnData);
    return IOConnectCallStructMethod(mConn, index, &keyData, sizeof(keyData), &returnData, &returnDataSize);
}
