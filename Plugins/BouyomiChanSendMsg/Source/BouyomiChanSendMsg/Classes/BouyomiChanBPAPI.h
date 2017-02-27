// Copyright 2015-2017 BlackMa9. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

#include "BouyomiChanBPAPI.generated.h"


#define D_BOUYOMICHAN_CMD_TALK				0x0001	/*CMD:読み上げ*/
#define D_BOUYOMICHAN_CMD_PAUSE				0x0010	/*CMD:一時停止*/
#define D_BOUYOMICHAN_CMD_RESUME			0x0020	/*CMD:一時停止の解除*/
#define D_BOUYOMICHAN_CMD_SKIP				0x0030	/*CMD:現在の行をスキップし次の行へ*/
#define D_BOUYOMICHAN_CMD_CLEAR				0x0040	/*CMD:残りタスクを全てキャンセル*/
#define D_BOUYOMICHAN_CMD_GETPAUSE			0x0110	/*CMD:一時停止状態の取得 -> 受信：一時停止状態8bit*/
#define D_BOUYOMICHAN_CMD_GETNOWPLAYING		0x0120	/*CMD:音声再生状態の取得 -> 受信：再生状態8bit*/
#define D_BOUYOMICHAN_CMD_GETTASKCOUNT		0x0130	/*CMD:残りタスク数の取得 -> 受信：残りタスク数32bit*/

/**
* BouyomiChan BluePrint API for socket send data 
*/
UCLASS()
class BOUYOMICHANSENDMSG_API  UBouyomiChanBPAPI : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/** Common IF: Send Raw Data For BouyomiChan Socket Commnication formats
	* @param targetIPAddr - Destination IP Address for ipv4
	* @param targetPort - Destination Port
	* @param sendData - send raw data
	* @param size - send raw data size (byte)
	*/
	static bool SendTo(
		FString targetIPAddr,
		int32 targetPort,
		uint8 * sendData,
		uint32 size
		);

	/** Send Message (value: sendMsgStr) for BoyomiChan. (Pursuant to BoyomiChan socket CMD )
	* @param targetIPAddr - Destination IP Address for ipv4 String (ex:127.0.0.1)
	* @param targetPort - Destination Port. (default : 50001)
	* @param sendMsgStr - send message
	* @param speedConf - Configure Speed ( range : 50 - 200 ( or -1:UserConf) , default 100)
	* @param toneConf - Configure Tone ( range : 50 - 200 ( or -1:UserConf), default 100)
	* @param volumeConf - Configure Volume ( range : 0 - 100 ( or -1:UserConf), default 100)
	* @param voiceConf - Configure Voice Actor Kind (0:UserConf, 1:女性1, 2:女性2, 3:男性1, 4:男性2, 5:中性, 6:ロボット, 7:機械1, 8:機械2, 10001～:SAPI )
	* @return result - The resulting send (true: OK)
	*/
	UFUNCTION(BlueprintCallable, Category = "BouyomiChanAPIBP")
		static bool SendMsg(
		FString targetIPAddr = "127.0.0.1",
		int32 targetPort = 50001,
		FString sendMsgStr = "Test Message",
		int32 speedConf = -1,
		int32 toneConf = -1,
		int32 volumeConf = -1,
		int32 voiceConf = 0
		);

	/** Send Pause Command for BoyomiChan.  
	* @param targetIPAddr - Destination IP Address for ipv4 String (ex:127.0.0.1)
	* @param targetPort - Destination Port. (default : 50001)
	* @return result - The resulting send (true: OK)
	*/
	UFUNCTION(BlueprintCallable, Category = "BouyomiChanAPIBP")
		static bool SendCmdPause(
		FString targetIPAddr = "127.0.0.1",
		int32 targetPort = 50001
		);

	/** Send Resume Command for BoyomiChan.
	* @param targetIPAddr - Destination IP Address for ipv4 String (ex:127.0.0.1)
	* @param targetPort - Destination Port. (default : 50001)
	* @return result - The resulting send (true: OK)
	*/
	UFUNCTION(BlueprintCallable, Category = "BouyomiChanAPIBP")
		static bool SendCmdResume(
		FString targetIPAddr = "127.0.0.1",
		int32 targetPort = 50001
		);

	/** Send Skip Command for BoyomiChan.
	* @param targetIPAddr - Destination IP Address for ipv4 String (ex:127.0.0.1)
	* @param targetPort - Destination Port. (default : 50001)
	* @return result - The resulting send (true: OK)
	*/
	UFUNCTION(BlueprintCallable, Category = "BouyomiChanAPIBP")
		static bool SendCmdSkip(
		FString targetIPAddr = "127.0.0.1",
		int32 targetPort = 50001
		);

	/** Send Clear Command for BoyomiChan.
	* @param targetIPAddr - Destination IP Address for ipv4 String (ex:127.0.0.1)
	* @param targetPort - Destination Port. (default : 50001)
	* @return result - The resulting send (true: OK)
	*/
	UFUNCTION(BlueprintCallable, Category = "BouyomiChanAPIBP")
		static bool SendCmdClear(
		FString targetIPAddr = "127.0.0.1",
		int32 targetPort = 50001
		);

	//////////////////////////////////////////////////////////////
	// General API
	//////////////////////////////////////////////////////////////


	/** Connection Test BP Node.
	* @param targetIPAddr - Destination IP Address for ipv4 String (ex:127.0.0.1)
	* @param targetPort - Destination Port. (default : 50001)
	* @return result - The resulting send (true: OK)
	*/
	UFUNCTION(BlueprintCallable, Category = "BouyomiChanAPIBP")
		static bool ConnectionTest(
		FString targetIPAddr = "127.0.0.1",
		int32 targetPort = 50001
		);

	/** GetNowPlaying (Cmd is send and Recv)
	* @param targetIPAddr - Destination IP Address for ipv4 String (ex:127.0.0.1)
	* @param targetPort - Destination Port. (default : 50001)
	* @return result - The resulting send (true: OK)
	*/
	UFUNCTION(BlueprintCallable, Category = "BouyomiChanAPIBP")
		static int32 GetNowPlaying(
		FString targetIPAddr = "127.0.0.1",
		int32 targetPort = 50001
		);
};