/*
 * Copyright (c) 2015 ARM Limited. All rights reserved.
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
#ifndef M2MNSDLINTERFACE_H
#define M2MNSDLINTERFACE_H

#include "mbed-client/m2mvector.h"
#include "mbed-client/m2mconfig.h"
#include "mbed-client/m2minterface.h"
#include "mbed-client/m2mtimerobserver.h"
#include "mbed-client/m2mobservationhandler.h"
#include "mbed-client/m2mbase.h"
#include "include/nsdllinker.h"

//FORWARD DECLARARTION
class M2MSecurity;
class M2MObject;
class M2MObjectInstance;
class M2MResource;
class M2MResourceInstance;
class M2MNsdlObserver;
class M2MServer;
class M2MTimer;

typedef Vector<M2MObject *> M2MObjectList;

/**
 * @brief M2MNsdlInterface
 * Class which interacts between mbed Client C++ Library and mbed-client-c library.
 */
class M2MNsdlInterface : public M2MTimerObserver,
                         public M2MObservationHandler
{
private:
    // Prevents the use of assignment operator by accident.
    M2MNsdlInterface& operator=( const M2MNsdlInterface& /*other*/ );

    // Prevents the use of copy constructor by accident
    M2MNsdlInterface( const M2MNsdlInterface& /*other*/ );

public:
    /**
    * @brief Constructor
    * @param observer, Observer to pass the event callbacks from nsdl library.
    */
    M2MNsdlInterface(M2MNsdlObserver &observer);

    /**
     * @brief Destructor
     */
    virtual ~M2MNsdlInterface();

    /**
     * @brief Creates endpoint object for the nsdl stack.
     * @param endpoint_name, Endpoint name of the client.
     * @param endpoint_type, Endpoint type of the client.
     * @param life_time, Life time of the client in seconds
     * @param domain, Domain of the client.
     * @param mode, Binding mode of the client, default is UDP
     * @param context_address, Context address default is empty.
    */
    void create_endpoint(const String &endpoint_name,
                         const String &endpoint_type,
                         const int32_t life_time,
                         const String &domain,
                         const uint8_t mode,
                         const String &context_address);

    /**
     * @brief Deletes the endpoint.
    */
    void delete_endpoint();

    /**
     * @brief Creates the NSDL structure for the registered objectlist.
     * @param object_list, List of objects to be registered.
     * @return true if structure created successfully else false.
    */
    bool create_nsdl_list_structure(const M2MObjectList &object_list);

    /**
     * @brief Removed the NSDL resource for the given resource.
     * @param resource_name, Resource name to be removed.
     * @return true if removed successfully else false.
    */
    bool delete_nsdl_resource(const String &resource_name);

    /**
     * @brief Creates the bootstrap object.
     * @param address Bootstrap address.
     * @return true if created and sent successfully else false.
    */
    bool create_bootstrap_resource(sn_nsdl_addr_s *address);

    /**
     * @brief Sends the register message to the server.
     * @param address M2MServer address.
     * @param port M2MServer port.
     * @param address_type IP Address type.
     * @return  true if register sent successfully else false.
    */
    bool send_register_message(uint8_t* address,
                               const uint16_t port,
                               sn_nsdl_addr_type_e address_type);

    /**
     * @brief Sends the update registration message to the server.
     * @param lifetime, Updated lifetime value in seconds.
     * @return  true if sent successfully else false.
     *
    */
    bool send_update_registration(const uint32_t lifetime = 0);

    /**
     * @brief Sends unregister message to the server.
     * @return  true if unregister sent successfully else false.
    */
    bool send_unregister_message();

    /**
     * @brief Memory Allocation required for libCoap.
     * @param size, Size of memory to be reserved.
    */
    void* memory_alloc(uint16_t size);

    /**
     * @brief Memory free functions required for libCoap
     * @param ptr, Object whose memory needs to be freed.
    */
    void memory_free(void *ptr);

    /**
    * @brief Callback from nsdl library to inform the data is ready
    * to be sent to server.
    * @param nsdl_handle, Handler for the nsdl structure for this endpoint
    * @param protocol, Protocol format of the data
    * @param data, Data to be sent.
    * @param data_len, Size of the data to be sent
    * @param address, server address where data has to be sent.
    * @return 1 if successful else 0.
    */
    uint8_t send_to_server_callback(struct nsdl_s * nsdl_handle,
                                    sn_nsdl_capab_e protocol,
                                    uint8_t *data,
                                    uint16_t data_len,
                                    sn_nsdl_addr_s *address);

    /**
    * @brief Callback from nsdl library to inform the data which is
    * received from server for the client has been converted to coap message.
    * @param nsdl_handle, Handler for the nsdl structure for this endpoint
    * @param coap_header, Coap message formed from data.
    * @param address, Server address from where the data is received.
    * @return 1 if successful else 0.
    */
    uint8_t received_from_server_callback(struct nsdl_s * nsdl_handle,
                                          sn_coap_hdr_s *coap_header,
                                          sn_nsdl_addr_s *address);

    /**
    * @brief Callback from nsdl library to inform the data which is
    * received from server for the resources has been converted to coap message.
    * @param nsdl_handle, Handler for the nsdl resource structure for this endpoint..
    * @param coap_header, Coap message formed from data.
    * @param address, Server address from where the data is received.
    * @param nsdl_capab, Protocol for the message, currently only coap is supported.
    * @return 1 if successful else 0.
    */
    uint8_t resource_callback(struct nsdl_s *nsdl_handle, sn_coap_hdr_s *coap,
                               sn_nsdl_addr_s *address,
                               sn_nsdl_capab_e nsdl_capab);

    /**
     * @brief Callback when the bootstrap information is received from bootstrap server.
     * @param server_info, Server information received from bootstrap server.
     */
    void bootstrap_done_callback(sn_nsdl_oma_server_info_t *server_info);

    /**
     * @brief Callback when there is data received from server and needs to be processed.
     * @param data, data received from server.
     * @param data_size, data size received from server.
     * @param addres, address structure of the server.
     * @return true if successfully processed else false.
     */
    bool process_received_data(uint8_t *data,
                               uint16_t data_size,
                               sn_nsdl_addr_s *address);

    /**
     * @brief Stops all the timers in case there is any errors.
     */
    void stop_timers();

protected: // from M2MTimerObserver

    virtual void timer_expired(M2MTimerObserver::Type type);

protected: // from M2MObservationHandler

    virtual void observation_to_be_sent(M2MBase *object,
                                        uint16_t obs_number,
                                        m2m::Vector<uint16_t> changed_instance_ids,
                                        bool send_object = false);

    virtual void resource_to_be_deleted(const String &resource_name);

    virtual void value_updated(M2MBase *base, const String &object_name);

    virtual void remove_object(M2MBase *object);

    virtual void send_delayed_response(M2MBase *base);

private:

    /**
    * @brief Initializes all the nsdl library component to be usable.
    * @return true if initialization is successful else false.
    */
    bool initialize();

    bool add_object_to_list(M2MObject *object);

    bool create_nsdl_object_structure(M2MObject *object);

    bool create_nsdl_object_instance_structure(M2MObjectInstance *object_instance);

    bool create_nsdl_resource_structure(M2MResource *resource,
                                        const String &object_name = "",
                                        bool multiple_instances = false);

    bool create_nsdl_resource(M2MBase *base, const String &name = "", bool publish_uri = true);

    String coap_to_string(uint8_t *coap_data_ptr,
                          int coap_data_ptr_length);

    void execute_nsdl_process_loop();

    uint64_t registration_time();

    M2MBase* find_resource(const String &object);

    M2MBase* find_resource(const M2MObject *object,
                           const String &object_instance);

    M2MBase* find_resource(const M2MObjectInstance *object_instance,
                           const String &resource_instance);

    M2MBase* find_resource(const M2MResource *resource,
                           const String &object_name,
                           const String &resource_instance);

    bool object_present(M2MObject * object) const;

    void clear_resource(sn_nsdl_resource_info_s *&resource);

    M2MInterface::Error interface_error(sn_coap_hdr_s *coap_header);

    void send_object_observation(M2MObject *object,
                                 uint16_t obs_number,
                                 m2m::Vector<uint16_t> changed_instance_ids,
                                 bool send_object);

    void send_object_instance_observation(M2MObjectInstance *object_instance,
                                          uint16_t obs_number);

    void send_resource_observation(M2MResource *resource, uint16_t obs_number);

    void build_observation_number(uint8_t *obs_number,
                                  uint8_t *obs_len,
                                  uint16_t number);

    void send_notification(uint8_t *token,
                           uint8_t  token_length,
                           uint8_t *value,
                           uint32_t value_length,
                           uint16_t observation,
                           uint32_t max_age,
                           uint8_t  coap_content_type,
                           const String  &uri_path);

private:

    M2MNsdlObserver                   &_observer;
    M2MObjectList                      _object_list;
    M2MServer                         *_server;
    M2MTimer                          *_nsdl_exceution_timer;
    M2MTimer                          *_registration_timer;
    sn_nsdl_ep_parameters_s           *_endpoint;
    sn_nsdl_resource_info_s           *_resource;
    sn_nsdl_bs_ep_info_t               _bootstrap_endpoint;
    sn_nsdl_oma_device_t               _bootstrap_device_setup;
    sn_nsdl_addr_s                     _sn_nsdl_address;
    nsdl_s                            *_nsdl_handle;
    uint32_t                           _counter_for_nsdl;
    int32_t                            _register_id;
    int32_t                            _unregister_id;
    uint16_t                           _update_id;
    uint16_t                           _bootstrap_id;

friend class Test_M2MNsdlInterface;

};

#endif // M2MNSDLINTERFACE_H
