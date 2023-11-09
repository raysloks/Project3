	void AcceptHandler(const asio::ip::udp::endpoint& endpoint);
	void ConnectHandler(const asio::ip::udp::endpoint& endpoint);
	void MpAuthenticationHandler(const asio::ip::udp::endpoint& endpoint, const MpAuthentication& message);
	void MpChatHandler(const asio::ip::udp::endpoint& endpoint, const MpChat& message);
	void MpCommandQueueAcknowledgeHandler(const asio::ip::udp::endpoint& endpoint, const MpCommandQueueAcknowledge& message);
	void MpDamageHandler(const asio::ip::udp::endpoint& endpoint, const MpDamage& message);
	void MpMobHealthUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobHealthUpdate& message);
	void MpMobSpriteUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobSpriteUpdate& message);
	void MpMobStatsUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobStatsUpdate& message);
	void MpMobTeamUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobTeamUpdate& message);
	void MpMobTypeUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobTypeUpdate& message);
	void MpMobUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpMobUpdate& message);
	void MpMobUpdateDataHandler(const asio::ip::udp::endpoint& endpoint, const MpMobUpdateData& message);
	void MpPlayerExperienceUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpPlayerExperienceUpdate& message);
	void MpPlayerInventoryUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpPlayerInventoryUpdate& message);
	void MpPlayerMobAbilitiesUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpPlayerMobAbilitiesUpdate& message);
	void MpPlayerMobCreatedHandler(const asio::ip::udp::endpoint& endpoint, const MpPlayerMobCreated& message);
	void MpSoundHandler(const asio::ip::udp::endpoint& endpoint, const MpSound& message);
	void MpUpdateHandler(const asio::ip::udp::endpoint& endpoint, const MpUpdate& message);
