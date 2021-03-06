	void AcceptHandler(const asio::ip::udp::endpoint& endpoint);
	void ConnectHandler(const asio::ip::udp::endpoint& endpoint);
	void MpChatHandler(const asio::ip::udp::endpoint& endpoint, const MpChat& message);
	void MpDamageHandler(const asio::ip::udp::endpoint& endpoint, const MpDamage& message);
	void MpMobHealthUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobHealthUpdate& message);
	void MpMobSpriteUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobSpriteUpdate& message);
	void MpMobStateUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobStateUpdate& message);
	void MpMobStatsUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobStatsUpdate& message);
	void MpMobTeamUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobTeamUpdate& message);
	void MpMobTypeUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobTypeUpdate& message);
	void MpMobUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobUpdate& message);
	void MpMobUpdateDataHandler(const asio::ip::udp::endpoint& endpoint, const MpMobUpdateData& message);
	void MpPlayerMobCreatedHandler(const asio::ip::udp::endpoint& endpoint, const MpPlayerMobCreated& message);
	void MpSoundHandler(const asio::ip::udp::endpoint& endpoint, const MpSound& message);
	void MpUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpUpdate& message);
