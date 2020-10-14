// Copyright 2019 GSI, Inc. All rights reserved.
//
//

#ifndef __ODC__CliServiceHelper__
#define __ODC__CliServiceHelper__

// ODC
#include "ControlService.h"
#include "Logger.h"
// STD
#include <chrono>
#include <iostream>
// BOOST
#include <boost/algorithm/string.hpp>

namespace odc
{
    namespace core
    {
        template <typename OwnerT>
        class CCliServiceHelper
        {
          public:
            /// \brief Run the service
            /// \param[in] _cmds Array of requests. If empty than command line input is required.
            /// \param[in] _delay Delay between command execution.
            void run(const std::vector<std::string>& _cmds = std::vector<std::string>(),
                     const std::chrono::milliseconds& _delay = std::chrono::milliseconds(1000))
            {
                printDescription();

                // Read the input from commnad line
                if (_cmds.empty())
                {
                    while (true)
                    {
                        std::string cmd;
                        OLOG(ESeverity::clean) << "Please enter command: ";
                        getline(std::cin, cmd);

                        // TODO: FIXME: for testing purposes we loop over m_partitionIDs and send the same request to
                        // different partitions. We need to find a better way to get a partition ID from CLI.
                        for (auto v : m_partitionIDs)
                        {
                            processRequest(v, cmd);
                        }
                    }
                }
                else
                {
                    // Execute consequently all commands
                    for (const auto& cmd : _cmds)
                    {
                        OLOG(ESeverity::clean) << "Executing command \"" << cmd << "\"";
                        // TODO: FIXME: for testing purposes we loop over m_partitionIDs and send the same request to
                        // different partitions. We need to find a better way to get a partition ID from CLI.
                        for (auto v : m_partitionIDs)
                        {
                            processRequest(v, cmd);
                        }
                        OLOG(ESeverity::clean) << "Waiting " << _delay.count() << " ms";
                        std::this_thread::sleep_for(_delay);
                    }
                    // Exit at the end
                    exit(EXIT_SUCCESS);
                }
            }

            void setPartitionIDs(const std::vector<partitionID_t>& _partitionIDs)
            {
                m_partitionIDs = _partitionIDs;
            }

            void setInitializeParams(const odc::core::SInitializeParams& _params)
            {
                m_initializeParams = _params;
            }
            void setSubmitParams(const odc::core::SSubmitParams& _params)
            {
                m_submitParams = _params;
            }
            void setActivateParams(const odc::core::SActivateParams& _params)
            {
                m_activateParams = _params;
            }
            void setUpscaleParams(const odc::core::SUpdateParams& _params)
            {
                m_upscaleParams = _params;
            }
            void setDownscaleParams(const odc::core::SUpdateParams& _params)
            {
                m_downscaleParams = _params;
            }
            void setRecoDeviceParams(const odc::core::SDeviceParams& _params)
            {
                m_recoDeviceParams = _params;
            }
            void setQCDeviceParams(const odc::core::SDeviceParams& _params)
            {
                m_qcDeviceParams = _params;
            }
            void setTimeout(const std::chrono::seconds& _timeout)
            {
                m_timeout = _timeout;
            }
            void setSetPropertiesParams(const odc::core::SSetPropertiesParams& _params)
            {
                m_setPropertiesParams = _params;
            }

          private:
            const odc::core::SDeviceParams& stringToDeviceParams(const std::string& _str)
            {
                if (_str == "reco")
                    return m_recoDeviceParams;
                else if (_str == "qc")
                    return m_qcDeviceParams;
                return m_allDeviceParams;
            }

            void processRequest(partitionID_t _partitionID, const std::string& _cmd)
            {
                if (_cmd == ".quit")
                {
                    exit(EXIT_SUCCESS);
                }

                OwnerT* p = reinterpret_cast<OwnerT*>(this);

                std::string replyString;

                std::vector<std::string> cmds;
                boost::split(cmds, _cmd, boost::is_any_of("\t "));
                std::string cmd{ cmds.empty() ? "" : cmds.front() };
                std::string par{ cmds.size() > 1 ? cmds[1] : "" };

                OLOG(ESeverity::clean) << "Requests for partition ID <" << _partitionID << ">";

                if (cmd == ".init")
                {
                    OLOG(ESeverity::clean) << "Sending initialization request...";
                    replyString = p->requestInitialize(_partitionID, m_initializeParams);
                }
                else if (cmd == ".submit")
                {
                    OLOG(ESeverity::clean) << "Sending submit request...";
                    replyString = p->requestSubmit(_partitionID, m_submitParams);
                }
                else if (cmd == ".activate")
                {
                    OLOG(ESeverity::clean) << "Sending activate request...";
                    replyString = p->requestActivate(_partitionID, m_activateParams);
                }
                else if (cmd == ".run")
                {
                    OLOG(ESeverity::clean) << "Sending run request...";
                    replyString = p->requestRun(_partitionID, m_initializeParams, m_submitParams, m_activateParams);
                }
                else if (cmd == ".upscale")
                {
                    OLOG(ESeverity::clean) << "Sending upscale request...";
                    replyString = p->requestUpscale(_partitionID, m_upscaleParams);
                }
                else if (cmd == ".downscale")
                {
                    OLOG(ESeverity::clean) << "Sending downscale request...";
                    replyString = p->requestDownscale(_partitionID, m_downscaleParams);
                }
                else if (cmd == ".config")
                {
                    OLOG(ESeverity::clean) << "Sending configure run request...";
                    replyString = p->requestConfigure(_partitionID, stringToDeviceParams(par));
                }
                else if (cmd == ".state")
                {
                    OLOG(ESeverity::clean) << "Sending get state request...";
                    replyString = p->requestGetState(_partitionID, stringToDeviceParams(par));
                }
                else if (cmd == ".prop")
                {
                    OLOG(ESeverity::clean) << "Sending set properties request...";
                    replyString = p->requestSetProperties(_partitionID, m_setPropertiesParams);
                }
                else if (cmd == ".start")
                {
                    OLOG(ESeverity::clean) << "Sending start request...";
                    replyString = p->requestStart(_partitionID, stringToDeviceParams(par));
                }
                else if (cmd == ".stop")
                {
                    OLOG(ESeverity::clean) << "Sending stop request...";
                    replyString = p->requestStop(_partitionID, stringToDeviceParams(par));
                }
                else if (cmd == ".reset")
                {
                    OLOG(ESeverity::clean) << "Sending reset request...";
                    replyString = p->requestReset(_partitionID, stringToDeviceParams(par));
                }
                else if (cmd == ".term")
                {
                    OLOG(ESeverity::clean) << "Sending terminate request...";
                    replyString = p->requestTerminate(_partitionID, stringToDeviceParams(par));
                }
                else if (cmd == ".down")
                {
                    OLOG(ESeverity::clean) << "Sending shutdown request...";
                    replyString = p->requestShutdown(_partitionID);
                }
                else
                {
                    OLOG(ESeverity::clean) << "Unknown command " << _cmd;
                }

                if (!replyString.empty())
                {
                    OLOG(ESeverity::clean) << "Reply: (\n" << replyString << ")";
                }
            }

            void printDescription()
            {
                OLOG(ESeverity::clean) << "Sample client for ODC service." << std::endl
                                       << "Available commands:" << std::endl
                                       << ".quit - Quit the program." << std::endl
                                       << ".init - Initialization request." << std::endl
                                       << ".submit - Submit request." << std::endl
                                       << ".activate - Activate request." << std::endl
                                       << ".run - Run request." << std::endl
                                       << ".prop - Set properties request." << std::endl
                                       << ".upscale - Upscale topology request." << std::endl
                                       << ".downscale - Downscale topology request." << std::endl
                                       << ".state (all|reco|qc) - Get state request." << std::endl
                                       << ".config (all|reco|qc) - Configure run request." << std::endl
                                       << ".start (all|reco|qc) - Start request." << std::endl
                                       << ".stop (all|reco|qc) - Stop request." << std::endl
                                       << ".reset (all|reco|qc) - Reset request." << std::endl
                                       << ".term (all|reco|qc) - Terminate request." << std::endl
                                       << ".down - Shutdown request." << std::endl;
            }

          private:
            std::vector<odc::core::partitionID_t> m_partitionIDs;
            odc::core::SInitializeParams m_initializeParams;
            odc::core::SSubmitParams m_submitParams;
            odc::core::SActivateParams m_activateParams;
            odc::core::SUpdateParams m_upscaleParams;
            odc::core::SUpdateParams m_downscaleParams;
            odc::core::SDeviceParams m_recoDeviceParams; ///< Parameters of Reco devices
            odc::core::SDeviceParams m_qcDeviceParams;   ///< Parameters of QC devices
            odc::core::SDeviceParams m_allDeviceParams;
            odc::core::SSetPropertiesParams m_setPropertiesParams;
            std::chrono::seconds m_timeout; ///< Request timeout
        };
    } // namespace core
} // namespace odc

#endif /* defined(__ODC__CliServiceHelper__) */
