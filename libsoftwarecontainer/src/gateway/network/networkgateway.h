/*
 * Copyright (C) 2016-2017 Pelagicore AB
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR
 * BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 * For further information see LICENSE
 */

#pragma once

#include "gateway/gateway.h"
#include "jansson.h"
#include "netlink.h"
#include "iptableentry.h"
#include "softwarecontainererror.h"
#include "networkgatewayfunctions.h"

namespace softwarecontainer {

/**
 * @brief Sets up and manages network access and routing to the container
 *
 * The responsibility of NetworkGateway is to setup network connection as specified by given
 * configuration. This configuration is described in detail in the user documentation, but
 * a short summary of it is how to handle incoming and outgoing network packages using the
 * three targets: ACCEPT, DROP and REJECT.
 */
class NetworkGateway :
        public Gateway
{
    LOG_DECLARE_CLASS_CONTEXT("NETG", "Network gateway");

public:
    static constexpr const char *ID = "network";

    // Exit codes used when running inside container
    static const constexpr int FAILURE = -1;
    static const constexpr int SUCCESS = 0;
    static const constexpr int NO_LINK = 1; // The desired link was not found
    static const constexpr int BAD_LINKUP = 2; // Could not bring link up
    static const constexpr int BAD_LINKDOWN = 3; // Could not bring link up

    /**
     * @brief Creates a network gateway
     *
     * @throws SoftwareContainerError if there is an error during initialization.
     */
    NetworkGateway(const int32_t id,
                   const std::string bridgeDevice,
                   const std::string gateway,
                   const uint8_t maskBits,
                   std::shared_ptr<ContainerAbstractInterface> container);

    virtual ~NetworkGateway();

    bool readConfigElement(const json_t *element) override;

    /**
     * @brief Implements Gateway::activateGateway
     */
    bool activateGateway() override;

    /**
     * @brief Implements Gateway::teardownGateway
     */
    bool teardownGateway() override;
private:
    /**
     * @brief Set route to default gateway
     *
     * Sets the route to the default gateway.
     * To be able to access anything outside the container, this method must be
     * called after the network interface has been enabled. This is also true for
     * cases when a network interface that was previously enabled has been disabled
     * and then enabled again.
     *
     * @return true on success false otherwise
     */
    bool setDefaultGateway();

    /**
     * @brief Enable the default network interface
     *
     * Enables the network interface and calls NetworkGateway::setDefaultGateway().
     *
     * When this is done for the first time, i.e. during the first call to activate()
     * the IP and netmask are also set. During subsequent calls, this merely brings
     * up the existing network interface and calls setDefaultGateway().
     *
     * @return true on success false otherwise
     */
    bool up();

    /**
     * @brief Disable the default network interface
     *
     * Disables the network interface.
     *
     * @return true on success
     * @return false otherwise
     */
    bool down();

    /**
     * @brief Check the availability of the network bridge on the host
     *
     * Checks the availability of the required network bridge on the host.
     *
     * @return true If bridge interface is available
     * @return false If bridge interface is not available
     */
    virtual bool isBridgeAvailable();

    struct in_addr m_ip;
    uint32_t m_netmask;
    std::string m_gateway;
    std::string m_bridgeDevice;

    std::vector<IPTableEntry> m_entries;

    bool m_interfaceInitialized;

    int32_t m_containerID;
    Netlink m_netlinkHost;

    NetworkGatewayFunctions m_functions;
};

class NetworkGatewayError : public SoftwareContainerError
{
public:
    NetworkGatewayError():
        m_message("NetworkGateway exception")
    {
    }

    NetworkGatewayError(const std::string &message):
        m_message(message)
    {
    }

    virtual const char *what() const throw()
    {
        return m_message.c_str();
    }

protected:
    std::string m_message;
};

class IPAllocationError : public NetworkGatewayError
{
public:
    IPAllocationError():
        NetworkGatewayError("An error is occurred when trying to generate IP address")
    {
    }

    IPAllocationError(const std::string &message):
        NetworkGatewayError(message)
    {
    }
};

} // namespace softwarecontainer
