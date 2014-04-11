/*
 *   Copyright (C) 2014 Pelagicore AB
 *   All rights reserved.
 */
#ifndef PULSEGATEWAY_H
#define PULSEGATEWAY_H

#include <pulse/pulseaudio.h>
#include "gateway.h"
#include "systemcallinterface.h"

/*! This Pelagicontain gateway is responsible for setting up a connection to the
 *  PulseAudio server running on the host system. The gateway decides whether to
 *  connect to the PulseAudio server or not based on the configuration passed to
 *  setConfig.
 *  If configured to enable audio, the gateway sets up a mainloop and then connects
 *  to the default PulseAudio server by calling pa_context_connect(). This is done
 *  during the activate() call.
 *  Once activate() has been called, the gateway listens to changes in the connection
 *  through the stateCallback function and, once the connection has been successfully
 *  set up, loads the module-native-protocol-unix PulseAudio module.
 *
 *  JSON format for configuration that enables audio (sent to setConfig()):
 *  \code{.js}
 *   {"audio": "true"}
 *  \endcode
 *
 *  A malformed configuration or a configuration that sets audio to false will simply
 *  disable audio and in such case, the gateway will not connect to the PulseAudio
 *  server at all.
 */
class PulseGateway:
    public Gateway
{
public:
    PulseGateway(
        const std::string &gatewayDir
        , const std::string &containerName
        , ControllerAbstractInterface *controllerInterface);
    ~PulseGateway();

    /*!
     *  Implements Gateway::id
     */
    virtual std::string id();

    /*!
     *  Implements Gateway::setConfig
     *
     *  Parses the JSON string configuration and sets m_enableAudio to true
     *  or false based on the content of the configuration.
     *
     * \param config JSON configuration object
     * \returns true upon successful parsing, false otherwise
     */
    virtual bool setConfig(const std::string &config);

    /*!
     *  Implements Gateway::activate
     *
     *  If audio is to be enabled, then calling this function results in a call
     *  to connectToPulseServer.
     *
     * \returns true upon success (PulseAudio server connect call and mainloop
     *               setup successfully), false otherwise.
     */
    virtual bool activate();

    /*! Implements Gateway::environment
     *
     *  This gateway has no environment.
     */
    virtual std::string environment();

private:

    enum ConfigError {Ok, BadConfig};

    /*! Creates a mainloop, sets a PulseAudio context, and sets callbacks for PulseAudio.
     *
     * \returns true upon success (PulseAudio server connect call and mainloop
     *               setup successfully), false otherwise.
     */
    bool connectToPulseServer();

    /*! Static callback called by PulseAudio when module has been loaded
     *
     * \param context The pa_context passed from PulseAudio
     * \param index The index of the loaded module
     * \param userdata Pointer to the calling PulseGateway object
     *
     */
    static void loadCallback(pa_context *context, uint32_t idx, void *userdata);

    /*! Static callback called by PulseAudio when module has been unloaded
     *
     * \param context The pa_context passed from PulseAudio
     * \param index The index of the loaded module
     * \param userdata Pointer to the calling PulseGateway object
     *
     */
    static void unloadCallback(pa_context *context, int success, void *userdata);

    /*! Static callback called by PulseAudio when the PulseAudio server has changed state
     *
     * \param context The pa_context passed from PulseAudio
     * \param index The index of the loaded module
     * \param userdata Pointer to the calling PulseGateway object
     *
     */
    static void stateCallback(pa_context *c, void *userdata);

    /*! Parse the JSON configuration passed down from Platform Access Manager
     *
     * Parses the configuration and looks up the value for the key passed as argument.
     *
     * \param config The JSON string containing the configuration
     * \param key The key to look up.
     * \return std::string  Value belonging to key
     * \return Empty string  Upon failure
     */
    std::string parseConfig(
        const std::string &config
        , const std::string &key
        , ConfigError *err = 0);

    /*! Returns the name of the socket, i.e. the filename part of the path
     *  to the socket.
     *
     * \return std::string The socket name
     */
    std::string socketName();

    pa_mainloop_api *m_api;
    pa_context *m_context;
    pa_threaded_mainloop *m_mainloop;
    std::string m_socket;
    int m_index;
    bool m_enableAudio;
    ControllerAbstractInterface *m_controllerInterface;
};

#endif /* PULSEGATEWAY_H */
