/*
 * Auto-generated from: 04_Industrial_Scenarios\10_Deep_Sea\01_Acoustic_Modem.md
 * Line: 431
 * Language: c
 * Block ID: 4e9e8cc8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ CP太短，无法覆盖多径时延
#define CP_LENGTH   64  // 仅4ms @ 16kHz
// 多径时延可达100ms

// ✅ CP应大于最大预期多径时延
#define CP_LENGTH   2048  // 128ms @ 16kHz
