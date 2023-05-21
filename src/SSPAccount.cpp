#include "tester/ssp/SSPAccount.hpp"

#include "tester/ssp/SSPCall.hpp"

SSPAccount::SSPAccount(const std::string &id, const std::string &domain, const std::string &secret,
                std::function<void(const pj::OnRegStateParam &)> onRegState,
                std::function<void(const pj::OnCallStateParam &, const pj::CallInfo &)> onCallState,
                std::function<void(const pj::CallInfo&, pj::Call *)> onIncomingCall):
    _onRegState(std::move(onRegState)),
    _onCallState(std::move(onCallState)),
    _onIncomingCall(std::move(onIncomingCall)),
    _call(nullptr)
{
    std::string uri = SIP + id + SEPARATOR + domain;
    _config.idUri = uri;
    _config.regConfig.registrarUri = SIP + domain;

    pj::AuthCredInfo aci(SCHEME, REALM, id, DATA_TYPE, secret);

    _config.sipConfig.authCreds.push_back(aci);
}

SSPAccount::~SSPAccount()
{
    // shutdown();
    LOG_INFO << "*** Account " + std::to_string(getId()) +" is being deleted ***" << std::endl;
    std::cout << "*** Account " + std::to_string(getId()) +" is being deleted ***" << std::endl;        
}

void SSPAccount::apply()
{
    try{
        create(_config);
    } catch (...) {
        LOG_ERORR << "Adding account failed" << std::endl;
        std::cerr << "Adding account failed" << std::endl;
    }
}

void SSPAccount::onRegState(pj::OnRegStateParam &prm) 
{   
    _onRegState(prm);
}

void SSPAccount::onIncomingCall(pj::OnIncomingCallParam &iprm)
{
    pj::Call* incomingCall = new SSPCall(*this, std::bind(&SSPAccount::onCallState, this,
            std::placeholders::_1), iprm.callId);

    auto ci = incomingCall->getInfo();

    _onIncomingCall(ci, incomingCall);
    
    // if(_call && _call->isActive())
    // {
    //     pj::CallOpParam opcode;
    //     opcode.statusCode = PJSIP_SC_BUSY_HERE;
    //     incomingCall->hangup(opcode);
    //     incomingCall = nullptr;
    //     return;
    // }
    // _call = incomingCall;
    // pj::CallInfo ci = _call->getInfo();
    // pj::CallOpParam prm;
    // LOG_INFO << "*** Incoming Call: " <<  ci.remoteUri << " ["
    //         << ci.stateText << "]" << std::endl;
    // std::cout << "*** Incoming Call: " <<  ci.remoteUri << " ["
    //         << ci.stateText << "]" << std::endl;
    // prm.statusCode = PJSIP_SC_OK;
    // _call->answer(prm);
}

void SSPAccount::onCallState(const pj::OnCallStateParam &prm)
{
    pj::CallInfo ci = _call->getInfo();
    if (ci.state == PJSIP_INV_STATE_DISCONNECTED)
    {
        // resetCall();
        _call = nullptr;
    }
    _onCallState(prm, ci);
}

void SSPAccount::call(const std::string & destUri)
{
    if(!_call)
    {
        _call = new SSPCall(*this, std::bind(&SSPAccount::onCallState, this,
                std::placeholders::_1));
        
    }
    if(getInfo().regIsActive && !_call->isActive())
    {
        pj::CallOpParam prm(true);
        prm.opt.audioCount = 1;
        prm.opt.videoCount = 0;
        _call->makeCall(destUri, prm);
    }
}

void SSPAccount::hangup()
{
    if(_call->isActive())
    {
        pj::CallOpParam opcode;
        opcode.statusCode = PJSIP_SC_DECLINE;
        _call->hangup(opcode);
    }
}

void SSPAccount::setCall(pj::Call* newCall)
{
    _call = newCall;
}

bool SSPAccount::activeCall()
{
    return _call && _call->isActive();
}

void SSPAccount::answer(const pj::CallOpParam& callOpParam)
{
    _call->answer(callOpParam);
}
