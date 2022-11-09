/*
 * Copyright (c) 2021-2022 Talkweb Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hal_sys_param.h"

static const char OHOS_DEVICE_TYPE[] = {"Evaluation Board"};
static const char OHOS_DISPLAY_VERSION[] = {"OpenHarmony 3.1"};
static const char OHOS_MANUFACTURE[] = {"Talkweb"};
static const char OHOS_BRAND[] = {"Talkweb"};
static const char OHOS_MARKET_NAME[] = {"Niobe"};
static const char OHOS_PRODUCT_SERIES[] = {"Niobe"};
static const char OHOS_PRODUCT_MODEL[] = {"Niobe407"};
static const char OHOS_SOFTWARE_MODEL[] = {"1.0.0"};
static const char OHOS_HARDWARE_MODEL[] = {"2.0.0"};
static const char OHOS_HARDWARE_PROFILE[] = {"RAM:192K,ROM:1M,ETH:true"};
static const char OHOS_BOOTLOADER_VERSION[] = {"twboot-v2022.03"};
static const char OHOS_ABI_LIST[] = {"armm4_hard_fpv4-sp-d16-liteos"};
static const char OHOS_SERIAL[] = {"1234567890"}; // provided by OEM.
static const int OHOS_FIRST_API_VERSION = 1;
static const char OHOS_SECURITY_PATCH_TAG[] = {"0.0.0.1"};
static const int OHOS_SDK_API_VERSION = 1;
static const char OHOS_VERSION_ID[] = {"0.0.0.1"};
static const char OHOS_BUILD_ROOT_HASH[] = {"de259d62a5dee08a2ee02fd7fc02caf8"};
static const char OHOS_OS_FULL_NAME[] = {"OpenHarmony"};

const char *GetDeviceType(void)
{
    return OHOS_DEVICE_TYPE;
}

const char *GetManufacture(void)
{
    return OHOS_MANUFACTURE;
}

const char *GetBrand(void)
{
    return OHOS_BRAND;
}

const char *GetMarketName(void)
{
    return OHOS_MARKET_NAME;
}

const char *GetProductSeries(void)
{
    return OHOS_PRODUCT_SERIES;
}

const char *GetProductModel(void)
{
    return OHOS_PRODUCT_MODEL;
}

const char *GetSoftwareModel(void)
{
    return OHOS_SOFTWARE_MODEL;
}

const char *GetHardwareModel(void)
{
    return OHOS_HARDWARE_MODEL;
}

const char *GetHardwareProfile(void)
{
    return OHOS_HARDWARE_PROFILE;
}

const char *GetSerial(void)
{
    return OHOS_SERIAL;
}

const char *GetBootloaderVersion(void)
{
    return OHOS_BOOTLOADER_VERSION;
}

const char *GetAbiList(void)
{
    return OHOS_ABI_LIST;
}

const char *GetDisplayVersion(void)
{
    return OHOS_DISPLAY_VERSION;
}

const char *GetIncrementalVersion(void)
{
    return INCREMENTAL_VERSION;
}

const char *GetBuildType(void)
{
    return BUILD_TYPE;
}

const char *GetBuildUser(void)
{
    return BUILD_USER;
}

const char *GetBuildHost(void)
{
    return BUILD_HOST;
}

const char *GetBuildTime(void)
{
    return BUILD_TIME;
}

int GetFirstApiVersion(void)
{
    return OHOS_FIRST_API_VERSION;
}

const char *GetSecurityPatchTag(void)
{
    return OHOS_SECURITY_PATCH_TAG;
}

int GetSdkApiVersion(void)
{
    return OHOS_SDK_API_VERSION;
}

const char *GetVersionId(void)
{
    return OHOS_VERSION_ID;
}

const char *GetBuildRootHash(void)
{
    return OHOS_BUILD_ROOT_HASH;
}

const char *GetOSFullName(void)
{
    return OHOS_OS_FULL_NAME;
}