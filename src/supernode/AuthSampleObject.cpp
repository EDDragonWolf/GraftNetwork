#include "AuthSampleObject.h"

void supernode::AuthSampleObject::Owner(AuthSample* o) { m_Owner = o; }

bool supernode::AuthSampleObject::Init(const RTA_TransactionRecord& src) {
	TransactionRecord = src;

	ADD_RTA_OBJECT_HANDLER(WalletProxyGetPosData, rpc_command::WALLET_GET_POS_DATA, AuthSampleObject);
	ADD_RTA_OBJECT_HANDLER(WalletProxyRejectPay, rpc_command::WALLET_REJECT_PAY, AuthSampleObject);

	return true;
}


bool supernode::AuthSampleObject::WalletProxyPay(const RTA_TransactionRecord& src, rpc_command::WALLET_PROXY_PAY::response& out) {
	if(src!=TransactionRecord) return false;

	//LOG_PRINT_L5("PaymentID: "<<TransactionRecord.PaymentID<<"  m_ReadyForDelete: "<<m_ReadyForDelete);

	// TODO: send LOCK. WTF?? all our nodes got this packet by sub-net broadcast. so only top node must send broad cast

	ADD_RTA_OBJECT_HANDLER(WalletPutTxInPool, rpc_command::WALLET_PUT_TX_IN_POOL, AuthSampleObject);

	out.Sign = GenerateSignForTransaction();
	out.FSN_StakeWalletAddr = m_Servant->GetMyStakeWallet().Addr;

	return true;
}

bool supernode::AuthSampleObject::WalletPutTxInPool(const rpc_command::WALLET_PUT_TX_IN_POOL::request& in, rpc_command::WALLET_PUT_TX_IN_POOL::response& out) {
	// all ok, notify PoS about this
	rpc_command::POS_TR_SIGNED::request req;
	rpc_command::POS_TR_SIGNED::response resp;
	req.TransactionPoolID = in.TransactionPoolID;
	if( !SendDAPICall(PosIP, PosPort, dapi_call::PoSTRSigned, req, resp) ) return false;


	return true;
}

bool supernode::AuthSampleObject::WalletProxyGetPosData(const rpc_command::WALLET_GET_POS_DATA::request& in, rpc_command::WALLET_GET_POS_DATA::response& out) {
    out.POSSaleDetails = TransactionRecord.POSSaleDetails;
	return true;
}

bool supernode::AuthSampleObject::WalletProxyRejectPay(const rpc_command::WALLET_REJECT_PAY::request &in, rpc_command::WALLET_REJECT_PAY::response &out) {
	rpc_command::WALLET_REJECT_PAY::request in2 = in;
	bool ret = SendDAPICall(PosIP, PosPort, dapi_call::AuthWalletRejectPay, in2, out);
	return ret;
}

string supernode::AuthSampleObject::GenerateSignForTransaction() {
	return m_Servant->SignByWalletPrivateKey( TransactionRecord.MessageForSign(), m_Servant->GetMyStakeWallet().Addr );
}


bool supernode::AuthSampleObject::Init(const RTA_TransactionRecordBase& src) { return false; }




