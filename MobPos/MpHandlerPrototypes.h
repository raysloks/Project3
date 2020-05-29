	void MpChatHandler(const asio::ip::udp::endpoint& endpoint, const MpChat& message);
	void MpGuidHandler(const asio::ip::udp::endpoint& endpoint, const MpGuid& message);
	void MpMobUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobUpdate& message);
	void MpMobUpdateDataHandler(const asio::ip::udp::endpoint& endpoint, const MpMobUpdateData& message);
	void MpPlayerMobCreatedHandler(const asio::ip::udp::endpoint& endpoint, const MpPlayerMobCreated& message);
	void MpSoundHandler(const asio::ip::udp::endpoint& endpoint, const MpSound& message);
	void MpUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpUpdate& message);
