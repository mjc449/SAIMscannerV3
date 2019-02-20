/*******************************************************************************
 * @file command_parser.h
 * @brief Function prototypes for parsing inbound USB packets
 * @author Marshall Colville (mjc449@cornell.edu)
 * 
 * Copyright 2018 Marshall Colville (mjc449@cornell.edu)
 * 
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*******************************************************************************/

#ifndef COMMAND_PARSER_H
#define	COMMAND_PARSER_H


//extern unsigned char ReceivedDataBuffer[64];
//extern unsigned char ToSendDataBuffer[64];


/*******************************************************************************
 * @brief Decides what to do when a new USB packet is received
 * 
 * Called on incoming USB packet.  Translates the incoming command and fills
 * the output buffer.  Some critical functions can disable all interrupts.  All
 * possible functions must set ToSendDataBuffer[0] = 0 prior to returning.
 ******************************************************************************/
void ParsePacketIn(void);


#endif	/* COMMAND_PARSER_H */

