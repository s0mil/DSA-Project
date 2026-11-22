#ifndef OTP_H
#define OTP_H
#include <string>
class OTP {
private:
    int otp;
    std::string lastError;
public:
    OTP();
    void generateOTP();
    bool sendOTP(std::string receiverEmail);
    bool verifyOTP(int inOTP);
    const std::string& getLastError() const;
};
#endif
