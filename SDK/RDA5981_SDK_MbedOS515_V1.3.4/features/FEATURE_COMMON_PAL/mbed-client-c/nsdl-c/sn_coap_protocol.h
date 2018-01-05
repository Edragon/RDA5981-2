/*
 * Copyright (c) 2011-2015 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * \file sn_coap_protocol.h
 *
 * \brief CoAP C-library User protocol interface header file
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SN_COAP_PROTOCOL_H_
#define SN_COAP_PROTOCOL_H_

#include "sn_coap_header.h"

/**
 * \fn struct coap_s *sn_coap_protocol_init(void* (*used_malloc_func_ptr)(uint16_t), void (*used_free_func_ptr)(void*),
        uint8_t (*used_tx_callback_ptr)(sn_nsdl_capab_e , uint8_t *, uint16_t, sn_nsdl_addr_s *),
        int8_t (*used_rx_callback_ptr)(sn_coap_hdr_s *, sn_nsdl_addr_s *)
 *
 * \brief Initializes CoAP Protocol part. When using libNsdl, sn_nsdl_init() calls this function.
 *
 * \param *used_malloc_func_ptr is function pointer for used memory allocation function.
 *
 * \param *used_free_func_ptr is function pointer for used memory free function.
 *
 * \param *used_tx_callback_ptr function callback pointer to tx function for sending coap messages
 *
 * \param *used_rx_callback_ptr used to return CoAP header struct with status COAP_STATUS_BUILDER_MESSAGE_SENDING_FAILED
 *        when re-sendings exceeded. If set to NULL, no error message is returned.
 *
 * \return  Pointer to handle when success
 *          Null if failed
 */

extern struct coap_s *sn_coap_protocol_init(void *(*used_malloc_func_ptr)(uint16_t), void (*used_free_func_ptr)(void *),
        uint8_t (*used_tx_callback_ptr)(uint8_t *, uint16_t, sn_nsdl_addr_s *, void *),
        int8_t (*used_rx_callback_ptr)(sn_coap_hdr_s *, sn_nsdl_addr_s *, void *));

/**
 * \fn int8_t sn_coap_protocol_destroy(void)
 *
 * \brief Frees all memory from CoAP protocol part
 *
 * \param *handle Pointer to CoAP library handle
 *
 * \return Return value is always 0
 */
extern int8_t sn_coap_protocol_destroy(struct coap_s *handle);

/**
 * \fn int16_t sn_coap_protocol_build(struct coap_s *handle, sn_nsdl_addr_s *dst_addr_ptr, uint8_t *dst_packet_data_ptr, sn_coap_hdr_s *src_coap_msg_ptr)
 *
 * \brief Builds Packet data from given CoAP header structure to be sent
 *
 * \param *dst_addr_ptr is pointer to destination address where CoAP message
 *        will be sent (CoAP builder needs that information for message resending purposes)
 *
 * \param *dst_packet_data_ptr is pointer to destination of built Packet data
 *
 * \param *src_coap_msg_ptr is pointer to source of built Packet data
 *
 * \param param void pointer that will be passed to tx/rx function callback when those are called.
 *
 * \return Return value is byte count of built Packet data.\n
 *         Note: If message is blockwised, all payload is not sent at the same time\n
 *         In failure cases:\n
 *          -1 = Failure in CoAP header structure\n
 *          -2 = Failure in given pointer (= NULL)\n
 *          -3 = Failure in Reset message\ŋ
 *         If there is not enough memory (or User given limit exceeded) for storing
 *         resending messages, situation is ignored.
 */
extern int16_t sn_coap_protocol_build(struct coap_s *handle, sn_nsdl_addr_s *dst_addr_ptr, uint8_t *dst_packet_data_ptr, sn_coap_hdr_s *src_coap_msg_ptr, void *param);

/**
 * \fn sn_coap_hdr_s *sn_coap_protocol_parse(struct coap_s *handle, sn_nsdl_addr_s *src_addr_ptr, uint16_t packet_data_len, uint8_t *packet_data_ptr)
 *
 * \brief Parses received CoAP message from given Packet data
 *
 * \param *src_addr_ptr is pointer to source address of received CoAP message
 *        (CoAP parser needs that information for Message acknowledgement)
 *
 * \param *handle Pointer to CoAP library handle
 *
 * \param packet_data_len is length of given Packet data to be parsed to CoAP message
 *
 * \param *packet_data_ptr is pointer to source of Packet data to be parsed to CoAP message
 *
 * \param param void pointer that will be passed to tx/rx function callback when those are called.
 *
 * \return Return value is pointer to parsed CoAP message structure. This structure includes also coap_status field.\n
 *         In following failure cases NULL is returned:\n
 *          -Given NULL pointer\n
 *          -Failure in parsed header of non-confirmable message\ŋ
 *          -Out of memory (malloc() returns NULL)
 */
extern sn_coap_hdr_s *sn_coap_protocol_parse(struct coap_s *handle, sn_nsdl_addr_s *src_addr_ptr, uint16_t packet_data_len, uint8_t *packet_data_ptr, void *);

/**
 * \fn int8_t sn_coap_protocol_exec(struct coap_s *handle, uint32_t current_time)
 *
 * \brief Sends CoAP messages from re-sending queue, if there is any.
 *        Cleans also old messages from the duplication list and from block receiving list
 *
 *        This function can be called e.g. once in a second but also more frequently.
 *
 * \param *handle Pointer to CoAP library handle
 *
 * \param current_time is System time in seconds. This time is
 *        used for message re-sending timing and to identify old saved data.
 *
 * \return  0 if success
 *          -1 if failed
 */

extern int8_t sn_coap_protocol_exec(struct coap_s *handle, uint32_t current_time);

/**
 * \fn int8_t sn_coap_protocol_set_block_size(uint16_t block_size)
 *
 * \brief If block transfer is enabled, this function changes the block size.
 *
 * \param uint16_t block_size maximum size of CoAP payload. Valid sizes are 16, 32, 64, 128, 256, 512 and 1024 bytes
 * \return  0 = success
 *          -1 = failure
 */
extern int8_t sn_coap_protocol_set_block_size(struct coap_s *handle, uint16_t block_size);

/**
 * \fn int8_t sn_coap_protocol_set_duplicate_buffer_size(uint8_t message_count)
 *
 * \brief If dublicate message detection is enabled, this function changes buffer size.
 *
 * \param uint8_t message_count max number of messages saved for duplicate control
 * \return  0 = success
 *          -1 = failure
 */
extern int8_t sn_coap_protocol_set_duplicate_buffer_size(struct coap_s *handle, uint8_t message_count);

/**
 * \fn int8_t sn_coap_protocol_set_retransmission_parameters(uint8_t resending_count, uint8_t resending_intervall)
 *
 * \brief  If re-transmissions are enabled, this function changes resending count and interval.
 *
 * \param uint8_t resending_count max number of resendings for message
 * \param uint8_t resending_intervall message resending intervall in seconds
 * \return  0 = success, -1 = failure
 */
extern int8_t sn_coap_protocol_set_retransmission_parameters(struct coap_s *handle,
        uint8_t resending_count, uint8_t resending_interval);

/**
 * \fn int8_t sn_coap_protocol_set_retransmission_buffer(uint8_t buffer_size_messages, uint16_t buffer_size_bytes)
 *
 * \brief If re-transmissions are enabled, this function changes message retransmission queue size.
 *  Set size to '0' to disable feature. If both are set to '0', then re-sendings are disabled.
 *
 * \param uint8_t buffer_size_messages queue size - maximum number of messages to be saved to queue
 * \param uint8_t buffer_size_bytes queue size - maximum size of messages saved to queue
 * \return  0 = success, -1 = failure
 */
extern int8_t sn_coap_protocol_set_retransmission_buffer(struct coap_s *handle,
        uint8_t buffer_size_messages, uint16_t buffer_size_bytes);

/**
 * \fn void sn_coap_protocol_clear_retransmission_buffer(struct coap_s *handle)
 *
 * \param *handle Pointer to CoAP library handle
 *
 * \brief If re-transmissions are enabled, this function removes all messages from the retransmission queue.
 */
extern void sn_coap_protocol_clear_retransmission_buffer(struct coap_s *handle);

#endif /* SN_COAP_PROTOCOL_H_ */

#ifdef __cplusplus
}
#endif