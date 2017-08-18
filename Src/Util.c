#include "Util.h"
/*
funName	:bufferCmp8
input		:uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength
output	:BOOLEAN ��ͬ����1����ͬ����0
describe:�����ַ����Ƚ�
remark	:
*/
BOOLEAN bufferCmp8(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FALSE;
    }

    pBuffer1++;
    pBuffer2++;
  }
  return TRUE;
}
/*
funName	:bufferCmp16
input		:uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength
output	:BOOLEAN ��ͬ����1����ͬ����0
describe:�����ַ����Ƚ�
remark	:
*/
BOOLEAN bufferCmp16(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FALSE;
    }

    pBuffer1++;
    pBuffer2++;
  }
  return TRUE;
}
