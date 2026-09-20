#pragma once
#include <string>

/** \brief Interface representing an email or notification service */
class MailService {
public:
    virtual ~MailService() = default;
    virtual void send(std::string message) = 0;
};

