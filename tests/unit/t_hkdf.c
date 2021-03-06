/*
 * Copyright 2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You may not
 * use this file except in compliance with the License. A copy of the License is
 * located at
 *
 *     http://aws.amazon.com/apache2.0/
 *
 * or in the "license" file accompanying this file. This file is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */
#include <aws/cryptosdk/error.h>
#include <aws/cryptosdk/private/hkdf.h>
#include "testing.h"
struct hkdf_test_vectors {
    enum aws_cryptosdk_sha_version which_sha;
    const uint8_t *ikm;
    size_t ikm_len;
    const uint8_t *salt;
    size_t salt_len;
    const uint8_t *info;
    size_t info_len;
    const uint8_t *okm_desired;
    size_t okm_len;
};

// Test vector 0: Basic test case with SHA-256
static const uint8_t tv_0_ikm[] = { 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
                                    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b };

static const uint8_t tv_0_salt[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c };

static const uint8_t tv_0_info[] = { 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9 };

static const uint8_t tv_0_okm_desired[] = { 0x3c, 0xb2, 0x5f, 0x25, 0xfa, 0xac, 0xd5, 0x7a, 0x90, 0x43, 0x4f,
                                            0x64, 0xd0, 0x36, 0x2f, 0x2a, 0x2d, 0x2d, 0x0a, 0x90, 0xcf, 0x1a,
                                            0x5a, 0x4c, 0x5d, 0xb0, 0x2d, 0x56, 0xec, 0xc4, 0xc5, 0xbf, 0x34,
                                            0x00, 0x72, 0x08, 0xd5, 0xb8, 0x87, 0x18, 0x58, 0x65 };

// Test vector 1:  Test with SHA-256 and longer inputs/outputs
static const uint8_t tv_1_ikm[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,
                                    0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
                                    0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
                                    0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                                    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45,
                                    0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f };

static const uint8_t tv_1_salt[] = { 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d,
                                     0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b,
                                     0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x82, 0x56, 0x85, 0x86, 0x87, 0x88, 0x89,
                                     0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
                                     0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5,
                                     0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf };

static const uint8_t tv_1_info[] = { 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd,
                                     0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb,
                                     0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9,
                                     0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
                                     0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5,
                                     0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff };

static const uint8_t tv_1_okm_desired[] = { 0x56, 0xa0, 0x54, 0x34, 0x1d, 0x0d, 0x47, 0xfa, 0x0b, 0x49, 0xd0, 0x1d,
                                            0x01, 0x35, 0x2d, 0xc2, 0x11, 0x0c, 0xfd, 0x75, 0x10, 0xfb, 0x06, 0x7c,
                                            0x9b, 0x5a, 0xe9, 0x69, 0x94, 0x56, 0x29, 0x63, 0x43, 0xc7, 0xfd, 0xd5,
                                            0xa9, 0xfe, 0xe2, 0x68, 0xd7, 0x9e, 0xea, 0xfa, 0x86, 0x3c, 0xf1, 0x17,
                                            0x58, 0xda, 0x18, 0xb0, 0x47, 0x88, 0xa0, 0xd2, 0xc5, 0x9f, 0x3b, 0x03,
                                            0x42, 0xa3, 0x82, 0x2e, 0xd7, 0xa6, 0xdf, 0xf1, 0x6c, 0x3b, 0x61, 0x3b,
                                            0x58, 0x9e, 0xcf, 0x0f, 0x71, 0x0b, 0xdf, 0x2b, 0x15, 0x39 };

// Test vector 2: Test with SHA-256 and zero-length salt/info
static const uint8_t tv_2_ikm[] = { 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
                                    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b };

static const uint8_t tv_2_okm_desired[] = { 0x8d, 0xa4, 0xe7, 0x75, 0xa5, 0x63, 0xc1, 0x8f, 0x71, 0x5f, 0x80,
                                            0x2a, 0x06, 0x3c, 0x5a, 0x31, 0xb8, 0xa1, 0x1f, 0x5c, 0x5e, 0xe1,
                                            0x87, 0x9e, 0xc3, 0x45, 0x4e, 0x5f, 0x3c, 0x73, 0x8d, 0x2d, 0x9d,
                                            0x20, 0x13, 0x95, 0xfa, 0xa4, 0xb6, 0x1a, 0x96, 0xc8 };

// Test vector 3: Basic test case with SHA-384
static const uint8_t tv_3_ikm[] = { 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
                                    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b };

static const uint8_t tv_3_salt[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c };

static const uint8_t tv_3_info[] = { 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9 };

static const uint8_t tv_3_okm_desired[] = { 0x9b, 0x50, 0x97, 0xa8, 0x60, 0x38, 0xb8, 0x05, 0x30, 0x90, 0x76,
                                            0xa4, 0x4b, 0x3a, 0x9f, 0x38, 0x06, 0x3e, 0x25, 0xb5, 0x16, 0xdc,
                                            0xbf, 0x36, 0x9f, 0x39, 0x4c, 0xfa, 0xb4, 0x36, 0x85, 0xf7, 0x48,
                                            0xb6, 0x45, 0x77, 0x63, 0xe4, 0xf0, 0x20, 0x4f, 0xc5 };

// Test vector 4:  Test with SHA-384 and longer inputs/outputs
static const uint8_t tv_4_ikm[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,
                                    0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
                                    0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
                                    0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                                    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45,
                                    0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f };

static const uint8_t tv_4_salt[] = { 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d,
                                     0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b,
                                     0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
                                     0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
                                     0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5,
                                     0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf };

static const uint8_t tv_4_info[] = { 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd,
                                     0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb,
                                     0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9,
                                     0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
                                     0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5,
                                     0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff };

static const uint8_t tv_4_okm_desired[] = { 0x48, 0x4c, 0xa0, 0x52, 0xb8, 0xcc, 0x72, 0x4f, 0xd1, 0xc4, 0xec, 0x64,
                                            0xd5, 0x7b, 0x4e, 0x81, 0x8c, 0x7e, 0x25, 0xa8, 0xe0, 0xf4, 0x56, 0x9e,
                                            0xd7, 0x2a, 0x6a, 0x05, 0xfe, 0x06, 0x49, 0xee, 0xbf, 0x69, 0xf8, 0xd5,
                                            0xc8, 0x32, 0x85, 0x6b, 0xf4, 0xe4, 0xfb, 0xc1, 0x79, 0x67, 0xd5, 0x49,
                                            0x75, 0x32, 0x4a, 0x94, 0x98, 0x7f, 0x7f, 0x41, 0x83, 0x58, 0x17, 0xd8,
                                            0x99, 0x4f, 0xdb, 0xd6, 0xf4, 0xc0, 0x9c, 0x55, 0x00, 0xdc, 0xa2, 0x4a,
                                            0x56, 0x22, 0x2f, 0xea, 0x53, 0xd8, 0x96, 0x7a, 0x8b, 0x2e };

// Test vector 5: Test with SHA-384 and zero-length salt/info
static const uint8_t tv_5_ikm[] = { 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
                                    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b };

static const uint8_t tv_5_okm_desired[] = { 0xc8, 0xc9, 0x6e, 0x71, 0x0f, 0x89, 0xb0, 0xd7, 0x99, 0x0b, 0xca,
                                            0x68, 0xbc, 0xde, 0xc8, 0xcf, 0x85, 0x40, 0x62, 0xe5, 0x4c, 0x73,
                                            0xa7, 0xab, 0xc7, 0x43, 0xfa, 0xde, 0x9b, 0x24, 0x2d, 0xaa, 0xcc,
                                            0x1c, 0xea, 0x56, 0x70, 0x41, 0x5b, 0x52, 0x84, 0x9c };

// Test vector 6: Test with NOSHA
static const uint8_t tv_6_ikm[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 };

struct hkdf_test_vectors tv[] = {
    { .which_sha   = AWS_CRYPTOSDK_SHA256,
      .ikm         = tv_0_ikm,
      .ikm_len     = 22,
      .salt        = tv_0_salt,
      .salt_len    = 13,
      .info        = tv_0_info,
      .info_len    = 10,
      .okm_desired = tv_0_okm_desired,
      .okm_len     = 42 },

    { .which_sha   = AWS_CRYPTOSDK_SHA256,
      .ikm         = tv_1_ikm,
      .ikm_len     = 80,
      .salt        = tv_1_salt,
      .salt_len    = 80,
      .info        = tv_1_info,
      .info_len    = 80,
      .okm_desired = tv_1_okm_desired,
      .okm_len     = 82 },

    { .which_sha   = AWS_CRYPTOSDK_SHA256,
      .ikm         = tv_2_ikm,
      .ikm_len     = 22,
      .salt        = NULL,
      .salt_len    = 0,
      .info        = NULL,
      .info_len    = 0,
      .okm_desired = tv_2_okm_desired,
      .okm_len     = 42 },

    { .which_sha   = AWS_CRYPTOSDK_SHA384,
      .ikm         = tv_3_ikm,
      .ikm_len     = 22,
      .salt        = tv_3_salt,
      .salt_len    = 13,
      .info        = tv_3_info,
      .info_len    = 10,
      .okm_desired = tv_3_okm_desired,
      .okm_len     = 42 },

    { .which_sha   = AWS_CRYPTOSDK_SHA384,
      .ikm         = tv_4_ikm,
      .ikm_len     = 80,
      .salt        = tv_4_salt,
      .salt_len    = 80,
      .info        = tv_4_info,
      .info_len    = 80,
      .okm_desired = tv_4_okm_desired,
      .okm_len     = 82 },

    { .which_sha   = AWS_CRYPTOSDK_SHA384,
      .ikm         = tv_5_ikm,
      .ikm_len     = 22,
      .salt        = NULL,
      .salt_len    = 0,
      .info        = NULL,
      .info_len    = 0,
      .okm_desired = tv_5_okm_desired,
      .okm_len     = 42 },

    { .which_sha = AWS_CRYPTOSDK_NOSHA, .ikm = tv_6_ikm, .ikm_len = 10 },
};

int test_hkdf() {
    for (int i = 0; i < 7; i++) {
        struct aws_byte_buf myokm;
        struct aws_allocator *allocator = aws_default_allocator();
        if (aws_byte_buf_init(&myokm, allocator, tv[i].okm_len)) return AWS_OP_ERR;
        myokm.len                        = tv[i].okm_len;
        const struct aws_byte_buf mysalt = aws_byte_buf_from_array(tv[i].salt, tv[i].salt_len);
        const struct aws_byte_buf myikm  = aws_byte_buf_from_array(tv[i].ikm, tv[i].ikm_len);
        const struct aws_byte_buf myinfo = aws_byte_buf_from_array(tv[i].info, tv[i].info_len);
        if (i == 6) {
            TEST_ASSERT_ERROR(
                AWS_CRYPTOSDK_ERR_UNSUPPORTED_FORMAT,
                aws_cryptosdk_hkdf(&myokm, tv[i].which_sha, &mysalt, &myikm, &myinfo));
        } else {
            if (aws_cryptosdk_hkdf(&myokm, tv[i].which_sha, &mysalt, &myikm, &myinfo)) return AWS_OP_ERR;
            if (memcmp(tv[i].okm_desired, myokm.buffer, myokm.len) != 0) return AWS_OP_ERR;
        }
        aws_byte_buf_clean_up(&myokm);
    }
    return AWS_OP_SUCCESS;
}

struct test_case hkdf_test_cases[] = { { "hkdf", "test_hkdf", test_hkdf }, { NULL } };
