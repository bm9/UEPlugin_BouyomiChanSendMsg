// Copyright 2015-2017 BlackMa9. All Rights Reserved.

#include "BouyomiChanSendMsgPrivatePCH.h"
#include "BouyomiChanBPAPI.h"


DEFINE_LOG_CATEGORY(LogBouyomiChanBPAPI)

UBouyomiChanBPAPI::UBouyomiChanBPAPI(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

///////////////////////////////////////////////////////////////////////////////////////////
//Bouyomi Chan Lapper API
//////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////
// Common IF : Send Raw Data
// note: 棒読みちゃん専用Socket向け送信用共通関数
//////////////////////////////////////////////////////
bool UBouyomiChanBPAPI::SendTo(
	FString targetIPAddr,
	int32 targetPort,
	uint8 * sendData,
	uint32 size
	)
{
	bool resultFlag = true;
	//param check
	if (targetIPAddr == "" || sendData == NULL || size == 0)
	{
		UE_LOG(LogBouyomiChanBPAPI, Error,
			TEXT("SendTo :: InParam is Err...IP[%s],Port[%d],DataPtr[0x%x],Size[%d]")
			, *targetIPAddr, targetPort, sendData, size);
		return false;
	}
	////////////////////////////////////////////
	// send data
	////////////////////////////////////////////
	FSocket * socket;
	FIPv4Address ip;
	FIPv4Address::Parse(targetIPAddr, ip);

	TSharedRef<FInternetAddr> addr
		= ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	addr->SetIp(ip.Value);
	addr->SetPort(targetPort);

	socket = FTcpSocketBuilder("boyomiSocket").AsNonBlocking().AsReusable().Build();
	
	if (socket->Connect(*addr))
	{
		int32 sent = 0;
		//data
		socket->Send( sendData, size ,sent);
#if 0 //Degbu Info
		UE_LOG(LogBouyomiChanBPAPI, Log, TEXT("SendTo :: Send API Comp."));
#endif
		//Close Socket
		socket->Close();
	}
	else
	{
		//接続失敗
		UE_LOG(LogBouyomiChanBPAPI, Error, TEXT("Socket Connection is Failer. ip(port)->[%s(%d)]"), *targetIPAddr, targetPort );

		//Close Socket (念のため？)
		socket->Close();
		resultFlag = false;
	}

	return resultFlag;
}

//////////////////////////////////////////////////
// Send Msg use Socket API
//////////////////////////////////////////////////////
bool UBouyomiChanBPAPI::SendMsg(
	FString targetIPAddr,
	int32 targetPort,
	FString sendMsgStr,
	int32 speedConf,
	int32 toneConf,
	int32 volumeConf,
	int32 voiceConf
	)
{
	bool resultFlag = true;

	//送信文字列が空欄の場合はSkipする
	if (sendMsgStr != "")
	{
#if 0
		//デバッグ用送信Msgの表示
		UE_LOG(LogBouyomiChanBPAPI, Log, TEXT("SendTargetMsg is [%s]"), *sendMsgStr);
#endif
		int32 sent = 0;
		int16 command = D_BOUYOMICHAN_CMD_TALK,
			speed = -1,
			tone = -1, 
			volume = -1, 
			voice = 0, 
			charcode = 1; //default

		speed = (0xFFFF & speedConf);
		tone = (0xFFFF & toneConf);
		volume = (0xFFFF & volumeConf);
		voice = (0xFFFF & voiceConf);
		TCHAR *sendData = sendMsgStr.GetCharArray().GetData();
		int32 size = sendMsgStr.GetAllocatedSize();

		//Msg ( UE4から送信するStringはUTF-8に変換せず、Unicode形式でそのまま送信する )

		uint8 * mallocBuf = (uint8*)FMemory::Malloc(size + 15);
		FMemory::Memcpy(&mallocBuf[0], &command, 2);
		FMemory::Memcpy(&mallocBuf[2], &speed, 2);
		FMemory::Memcpy(&mallocBuf[4], &tone, 2);
		FMemory::Memcpy(&mallocBuf[6], &volume, 2);
		FMemory::Memcpy(&mallocBuf[8], &voice, 2);
		FMemory::Memcpy(&mallocBuf[10], &charcode, 1);
		FMemory::Memcpy(&mallocBuf[11], &size, 4);
		FMemory::Memcpy(&mallocBuf[15], sendData, size);
#if 0
		UE_LOG(LogBouyomiChanBPAPI, Log, TEXT("SendTargetMsg Conf: speed[%d]-speedConf[%d]->cmd[%x]"), speed, speedConf, *((short*)&cmdBuf[2]));
		UE_LOG(LogBouyomiChanBPAPI, Log, TEXT("SendTargetMsg Conf: tone[%d]-toneConf[%d]->cmd[%x]"), tone, toneConf, *((short*)&cmdBuf[4]));
		UE_LOG(LogBouyomiChanBPAPI, Log, TEXT("SendTargetMsg Conf: volume[%d]-volumeConf[%d]->cmd[%x]"), volume, volumeConf, *((short*)&cmdBuf[6]));
		UE_LOG(LogBouyomiChanBPAPI, Log, TEXT("SendTargetMsg Conf: voice[%d]-voiceConf[%d]->cmd[%x]"), voice, voiceConf, *((short*)&cmdBuf[8]));
		UE_LOG(LogBouyomiChanBPAPI, Log, TEXT("SendTargetMsg Conf: charcode[%d]->cmd[%x]"), charcode, *((uint8*)&cmdBuf[10]));
		UE_LOG(LogBouyomiChanBPAPI, Log, TEXT("SendTargetMsg Conf: size[%d]->cmd[%x]"), size, *((int32*)&cmdBuf[11]));

		{
			UE_LOG(LogBouyomiChanBPAPI, Log, TEXT("-----MsgCnf--------------"));
			FString dump;
			for (int32 m = 0; m < 15; ++m)
			{
				char temp[4] = { 0 };
				sprintf(temp, "%02X ", mallocBuf[m]);
				dump += FString(temp);
				if (m % 16 == 15)
				{
					UE_LOG(LogBouyomiChanBPAPI, Log, TEXT("%s"), *dump);
					dump = "";
				}
			}
			if (dump != "")
			{
				UE_LOG(LogBouyomiChanBPAPI, Log, TEXT("%s"), *dump);
			}
			UE_LOG(LogBouyomiChanBPAPI, Log, TEXT("-----MsgCnf--------------"));
		}
#endif
		resultFlag = SendTo(targetIPAddr, targetPort, mallocBuf, 15 + size);
		if(resultFlag == false)
		{
			UE_LOG(LogBouyomiChanBPAPI, Error, TEXT("SendVoiceTextData :: Send Error."));
		}

		FMemory::Free(mallocBuf);
		//UE_LOG(LogBouyomiChanBPAPI, Log, TEXT("VoiceSend :: Send Comp."));
	}
	else
	{
		//空欄の場合は送信しない
#if 0
		UE_LOG(LogBouyomiChanBPAPI, Warning, TEXT("SendTargetMsg is Empty."));
#endif
	}
	//UE_LOG(LogBouyomiChanBPAPI, Log, TEXT("SendFunc is end "));
	return resultFlag;
}


///////////////////////////////////////
// 棒読みちゃん CMD：Pause（一時停止）
//////////////////////////////////////
bool UBouyomiChanBPAPI::SendCmdPause(
	FString targetIPAddr,
	int32 targetPort
	)
{
	uint16 cmd = D_BOUYOMICHAN_CMD_PAUSE; //Pause
	bool resultFlag;
	resultFlag = SendTo(targetIPAddr, targetPort, (uint8*)(&cmd), 2);
	if(resultFlag == false)
	{
		UE_LOG(LogBouyomiChanBPAPI, Error, TEXT("SendCmdPause :: Send Error."));
	}
	return resultFlag;
}
///////////////////////////////////////
// 棒読みちゃん CMD：Resume（一時停止の解除）
//////////////////////////////////////
bool UBouyomiChanBPAPI::SendCmdResume(
	FString targetIPAddr,
	int32 targetPort
	)
{
	uint16 cmd = D_BOUYOMICHAN_CMD_RESUME; //Resume
	bool resultFlag;
	resultFlag = SendTo(targetIPAddr, targetPort, (uint8*)(&cmd), 2);
	if(resultFlag == false)
	{
		UE_LOG(LogBouyomiChanBPAPI, Error, TEXT("SendCmdResume :: Send Error."));
	}
	return resultFlag;
}
///////////////////////////////////////
// 棒読みちゃん CMD：Skip現在の行をスキップし次の行へ）
//////////////////////////////////////
bool UBouyomiChanBPAPI::SendCmdSkip(
	FString targetIPAddr,
	int32 targetPort
	)
{
	uint16 cmd = D_BOUYOMICHAN_CMD_SKIP; //Skip
	bool resultFlag;
	resultFlag = SendTo(targetIPAddr, targetPort, (uint8*)(&cmd), 2);
	if(resultFlag == false)
	{
		UE_LOG(LogBouyomiChanBPAPI, Error, TEXT("SendCmdSkip :: Send Error."));
	}
	return resultFlag;
}
///////////////////////////////////////
// 棒読みちゃん CMD：Clear （残りタスクを全てキャンセル）
//////////////////////////////////////
bool UBouyomiChanBPAPI::SendCmdClear(
	FString targetIPAddr,
	int32 targetPort
	)
{
	uint16 cmd = D_BOUYOMICHAN_CMD_CLEAR; //Clear
	bool resultFlag;
	resultFlag = SendTo(targetIPAddr, targetPort, (uint8*)(&cmd), 2);
	if(resultFlag == false)
	{
		UE_LOG(LogBouyomiChanBPAPI, Error, TEXT("SendCmdClear :: Send Error."));
	}
	return resultFlag;
}


//////////////////////////////////////////////////////////////
// General API
//////////////////////////////////////////////////////////////

/** Connection Test BP Node.
* @param targetIPAddr - Destination IP Address for ipv4 String (ex:127.0.0.1)
* @param targetPort - Destination Port. (default : 50001)
* @return result - The resulting send (true: OK)
*/
bool UBouyomiChanBPAPI::ConnectionTest(
	FString targetIPAddr,
	int32 targetPort
	)
{

	bool resultFlag = true;
	//param check
	if (targetIPAddr == "" )
	{
		UE_LOG(LogBouyomiChanBPAPI, Error,
			TEXT("ConnectionTest :: InParam is Err...IP[%s],Port[%d]")
			, *targetIPAddr, targetPort);
		return false;
	}
	////////////////////////////////////////////
	// send data
	////////////////////////////////////////////
	FSocket * socket;
	FIPv4Address ip;
	FIPv4Address::Parse(targetIPAddr, ip);

	TSharedRef<FInternetAddr> addr
		= ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	addr->SetIp(ip.Value);
	addr->SetPort(targetPort);

	socket = FTcpSocketBuilder("boyomiSocket").AsNonBlocking().AsReusable().Build();

	if (socket->Connect(*addr))
	{
		resultFlag = true;
	}
	else
	{
		//接続失敗
		UE_LOG(LogBouyomiChanBPAPI, Error, TEXT("ConnectionTest :: Socket Connection is Failer. ip(port)->[%s(%d)]"), *targetIPAddr, targetPort);
		resultFlag = false;
	}

	//Close Socket
	socket->Close();
	return resultFlag;
}


/** GetNowPlaying (Cmd is send and Recv)
* @param targetIPAddr - Destination IP Address for ipv4 String (ex:127.0.0.1)
* @param targetPort - Destination Port. (default : 50001)
* @return result - The resulting send (true: OK)
*/
int32 UBouyomiChanBPAPI::GetNowPlaying(
	FString targetIPAddr,
	int32 targetPort
	)
{

	uint16 cmd = D_BOUYOMICHAN_CMD_GETNOWPLAYING;
	uint32 size = 2;
	uint8 * sendData = (uint8*)(&cmd);

	//////////////////////////////

	int32 resultFlag = -1;
	//param check
	if (targetIPAddr == "")
	{
		UE_LOG(LogBouyomiChanBPAPI, Error,
			TEXT("ConnectionTest :: InParam is Err...IP[%s],Port[%d]")
			, *targetIPAddr, targetPort);
		return resultFlag;
	}
	////////////////////////////////////////////
	// send data
	////////////////////////////////////////////
	FSocket * socket;
	FIPv4Address ip;
	FIPv4Address::Parse(targetIPAddr, ip);

	TSharedRef<FInternetAddr> addr
		= ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	addr->SetIp(ip.Value);
	addr->SetPort(targetPort);

	socket = FTcpSocketBuilder("boyomiSocket").AsNonBlocking().AsReusable().Build();

	if (socket->Connect(*addr))
	{
		int32 sent = 0;
		//data
		socket->Send(sendData, size, sent);

		//////////////////////////////////
		//Recv
		TArray<uint8> ReceivedData;
		int32 BufferSize = 1024;
		uint32 SizeRecv;
		bool bIsDataReceived = false;
		bool bIsPendingConnection = true;
		while (!bIsDataReceived)
		{
			if (socket->GetConnectionState() != ESocketConnectionState::SCS_Connected)
			{
				//該当するかは不明…念の為に？
				break;
			}
			if (socket->HasPendingData(SizeRecv))
			{
				ReceivedData.Init(0,FMath::Min(SizeRecv, 65507u));

				int32 Read = 0;
				socket->RecvFrom(ReceivedData.GetData(), BufferSize, Read, *addr);

				if (Read > 0)
				{
					resultFlag = ReceivedData[0];
					bIsDataReceived = true;

					/*Debug Dump*/
					if(1)
					{
						UE_LOG(LogBouyomiChanBPAPI, Log, TEXT("-----MsgCnf--------------"));
						FString dump;
						for (int32 m = 0; m < Read; ++m)
						{
							char temp[4] = { 0 };
							sprintf(temp, "%02X ", ReceivedData[m]);
							dump += FString(temp);
							if (m % 16 == 15)
							{
								UE_LOG(LogBouyomiChanBPAPI, Log, TEXT("%s"), *dump);
								dump = "";
							}
						}
						if (dump != "")
						{
							UE_LOG(LogBouyomiChanBPAPI, Log, TEXT("%s"), *dump);
						}
						UE_LOG(LogBouyomiChanBPAPI, Log, TEXT("-----MsgCnf--------------"));
					}
				}

			}
		}
		//////////////////////////////////

	}
	else
	{
		//接続失敗
		UE_LOG(LogBouyomiChanBPAPI, Error, TEXT("ConnectionTest :: Socket Connection is Failer. ip(port)->[%s(%d)]"), *targetIPAddr, targetPort);
		resultFlag = -1;
	}

	//Close Socket
	socket->Close();
	return resultFlag;
}