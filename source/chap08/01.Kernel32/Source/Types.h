/**
 *  file    Types.h
 *  date    2008/12/14
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   커널에서 사용하는 각종 타입을 정의한 파일
 */

#ifndef __TYPES_H__
#define __TYPES_H__

////////////////////////////////////////////////////////////////////////////////
//
// 매크로
//
////////////////////////////////////////////////////////////////////////////////
#define uint8_t    unsigned char
#define uint16_t    unsigned short
#define uint32_t   unsigned int
#define QWORD   unsigned long
#define bool    unsigned char

#define true    1
#define false   0
#define NULL    0


////////////////////////////////////////////////////////////////////////////////
//
// 구조체
//
////////////////////////////////////////////////////////////////////////////////

#define __packed __attribute__((packed))
// 비디오 모드 중 텍스트 모드 화면을 구성하는 자료구조
typedef struct kCharactorStruct
{
    uint8_t bCharactor;
    uint8_t bAttribute;
} __packed CHARACTER;


#endif /*__TYPES_H__*/
