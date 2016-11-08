#include "softwarecontaineragentadaptor.h"

namespace softwarecontainer {

SoftwareContainerAgentAdaptor::~SoftwareContainerAgentAdaptor()
{
}

SoftwareContainerAgentAdaptor::SoftwareContainerAgentAdaptor(SoftwareContainerAgent &agent) :
    m_agent(agent)
{
}

int32_t SoftwareContainerAgentAdaptor::LaunchCommand(
    const int32_t &containerID,
    const uint32_t &userID,
    const std::string &commandLine,
    const std::string &workingDirectory,
    const std::string &outputFile,
    const std::map<std::string, std::string> &env)
{
    return m_agent.launchCommand(
        containerID,
        userID,
        commandLine,
        workingDirectory,
        outputFile,
        env,
        [this, containerID](pid_t pid, int exitCode) {
            ProcessStateChanged(containerID, pid, false, exitCode);
            log_info() << "ProcessStateChanged " << pid << " code " << exitCode;
        }
    );
}

bool SoftwareContainerAgentAdaptor::SuspendContainer(const int32_t &containerID)
{
    return m_agent.suspendContainer(containerID);
}

bool SoftwareContainerAgentAdaptor::ResumeContainer(const int32_t &containerID)
{
    return m_agent.resumeContainer(containerID);
}

bool SoftwareContainerAgentAdaptor::ShutDownContainer(const int32_t &containerID)
{
    return m_agent.shutdownContainer(containerID);
}

bool SoftwareContainerAgentAdaptor::ShutDownContainerWithTimeout(
    const int32_t &containerID,
    const uint32_t &timeout)
{
    return m_agent.shutdownContainer(containerID, timeout);
}

std::string SoftwareContainerAgentAdaptor::BindMountFolderInContainer(
    const int32_t &containerID,
    const std::string &pathInHost,
    const std::string &subPathInContainer,
    const bool &readOnly)
{
    return m_agent.bindMountFolderInContainer(containerID,
                                              pathInHost,
                                              subPathInContainer,
                                              readOnly);
}

void SoftwareContainerAgentAdaptor::SetGatewayConfigs(
    const int32_t &containerID,
    const std::map<std::string, std::string> &configs)
{
    m_agent.setGatewayConfigs(containerID, configs);
}

bool SoftwareContainerAgentAdaptor::SetCapabilities(
    const int32_t &containerID,
    const std::vector<std::string> &capabilities)
{
    return m_agent.setCapabilities(containerID, capabilities);
}

int32_t SoftwareContainerAgentAdaptor::CreateContainer(const std::string &config)
{
    return m_agent.createContainer(config);
}

bool SoftwareContainerAgentAdaptor::SetContainerName(
    const int32_t &containerID,
    const std::string &name)
{
    return m_agent.setContainerName(containerID, name);
}

void SoftwareContainerAgentAdaptor::Ping()
{
}

bool SoftwareContainerAgentAdaptor::WriteToStdIn(
    const uint32_t &processID,
    const std::vector<uint8_t> &bytes)
{
    return m_agent.writeToStdIn(processID, bytes);
}

} // End namespace softwarecontainer