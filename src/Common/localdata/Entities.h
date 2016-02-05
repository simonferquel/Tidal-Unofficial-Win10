#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <sqlite3.h>
#include <LocalDB/DBQuery.h>
#include <LocalDB/Sqlite3Extensions.h>
namespace localdata {
	struct cached_track {
		std::int64_t id;
		std::int64_t quality;
		std::int64_t server_timestamp;
		std::int64_t server_size;
		std::int64_t local_size;
		std::int64_t last_playpack_time;
		std::int64_t obuscated;
		static std::string getOrderedColumnsForSelect() {
			return "id, quality, server_timestamp, server_size, local_size, last_playpack_time, obuscated";
		}
		static cached_track createFromSqlRecord(sqlite3_stmt* stmt) {
			cached_track data;
			data.id = sqlite3_column_int64(stmt, 0);
			data.quality = sqlite3_column_int64(stmt, 1);
			data.server_timestamp = sqlite3_column_int64(stmt, 2);
			data.server_size = sqlite3_column_int64(stmt, 3);
			data.local_size = sqlite3_column_int64(stmt, 4);
			data.last_playpack_time = sqlite3_column_int64(stmt, 5);
			data.obuscated = sqlite3_column_int64(stmt, 6);
			return data;
		}
	};
	class cached_trackInsertDbQuery : public LocalDB::NoResultDBQuery {
	private:
		cached_track _entity;
	protected:
		virtual std::string identifier() override { return "gen-cached_track-insert"; }
		virtual std::string sql(int) override {
			return "insert into cached_track(id, quality, server_timestamp, server_size, local_size, last_playpack_time, obuscated) values (@id, @quality, @server_timestamp, @server_size, @local_size, @last_playpack_time, @obuscated)";
		}
		virtual  void bindParameters(int, sqlite3*, sqlite3_stmt* statement) override {
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@id"), _entity.id);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@quality"), _entity.quality);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@server_timestamp"), _entity.server_timestamp);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@server_size"), _entity.server_size);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@local_size"), _entity.local_size);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@last_playpack_time"), _entity.last_playpack_time);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@obuscated"), _entity.obuscated);
		}
	public:
		cached_trackInsertDbQuery(const LocalDB::DBContext& ctx, const cached_track& entity) : LocalDB::NoResultDBQuery(ctx), _entity(entity) {}
	};
	class cached_trackInsertOrReplaceDbQuery : public LocalDB::NoResultDBQuery {
	private:
		cached_track _entity;
	protected:
		virtual std::string identifier() override { return "gen-cached_track-insertorreplace"; }
		virtual std::string sql(int) override {
			return "insert or replace into cached_track(id, quality, server_timestamp, server_size, local_size, last_playpack_time, obuscated) values (@id, @quality, @server_timestamp, @server_size, @local_size, @last_playpack_time, @obuscated)";
		}
		virtual  void bindParameters(int, sqlite3*, sqlite3_stmt* statement) override {
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@id"), _entity.id);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@quality"), _entity.quality);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@server_timestamp"), _entity.server_timestamp);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@server_size"), _entity.server_size);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@local_size"), _entity.local_size);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@last_playpack_time"), _entity.last_playpack_time);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@obuscated"), _entity.obuscated);
		}
	public:
		cached_trackInsertOrReplaceDbQuery(const LocalDB::DBContext& ctx, const cached_track& entity) : LocalDB::NoResultDBQuery(ctx), _entity(entity) {}
	};
	class cached_trackUpdateDbQuery : public LocalDB::NoResultDBQuery {
	private:
		cached_track _entity;
	protected:
		virtual std::string identifier() override { return "gen-cached_track-update"; }
		virtual std::string sql(int) override {
			return "update cached_track SET quality = @quality, server_timestamp = @server_timestamp, server_size = @server_size, local_size = @local_size, last_playpack_time = @last_playpack_time, obuscated = @obuscated WHERE id = @id";
		}
		virtual  void bindParameters(int, sqlite3*, sqlite3_stmt* statement) override {
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@id"), _entity.id);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@quality"), _entity.quality);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@server_timestamp"), _entity.server_timestamp);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@server_size"), _entity.server_size);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@local_size"), _entity.local_size);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@last_playpack_time"), _entity.last_playpack_time);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@obuscated"), _entity.obuscated);
		}
	public:
		cached_trackUpdateDbQuery(const LocalDB::DBContext& ctx, const cached_track& entity) : LocalDB::NoResultDBQuery(ctx), _entity(entity) {}
	};
	struct imported_track {
		std::int64_t id;
		std::int64_t quality;
		std::int64_t last_playpack_time;
		std::int64_t album_id;
		std::wstring album_title;
		std::wstring cover;
		std::wstring title;
		std::wstring artist;
		std::wstring json;
		std::int64_t import_timestamp;
		std::int64_t size;
		std::int64_t obuscated;
		static std::string getOrderedColumnsForSelect() {
			return "id, quality, last_playpack_time, album_id, album_title, cover, title, artist, json, import_timestamp, size, obuscated";
		}
		static imported_track createFromSqlRecord(sqlite3_stmt* stmt) {
			imported_track data;
			data.id = sqlite3_column_int64(stmt, 0);
			data.quality = sqlite3_column_int64(stmt, 1);
			data.last_playpack_time = sqlite3_column_int64(stmt, 2);
			data.album_id = sqlite3_column_int64(stmt, 3);
			data.album_title = LocalDB::sqlite3_column_wstring(stmt, 4);
			data.cover = LocalDB::sqlite3_column_wstring(stmt, 5);
			data.title = LocalDB::sqlite3_column_wstring(stmt, 6);
			data.artist = LocalDB::sqlite3_column_wstring(stmt, 7);
			data.json = LocalDB::sqlite3_column_wstring(stmt, 8);
			data.import_timestamp = sqlite3_column_int64(stmt, 9);
			data.size = sqlite3_column_int64(stmt, 10);
			data.obuscated = sqlite3_column_int64(stmt, 11);
			return data;
		}
	};
	class imported_trackInsertDbQuery : public LocalDB::NoResultDBQuery {
	private:
		imported_track _entity;
	protected:
		virtual std::string identifier() override { return "gen-imported_track-insert"; }
		virtual std::string sql(int) override {
			return "insert into imported_track(id, quality, last_playpack_time, album_id, album_title, cover, title, artist, json, import_timestamp, size, obuscated) values (@id, @quality, @last_playpack_time, @album_id, @album_title, @cover, @title, @artist, @json, @import_timestamp, @size, @obuscated)";
		}
		virtual  void bindParameters(int, sqlite3*, sqlite3_stmt* statement) override {
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@id"), _entity.id);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@quality"), _entity.quality);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@last_playpack_time"), _entity.last_playpack_time);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@album_id"), _entity.album_id);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@album_title"), _entity.album_title);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@cover"), _entity.cover);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@title"), _entity.title);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@artist"), _entity.artist);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@json"), _entity.json);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@import_timestamp"), _entity.import_timestamp);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@size"), _entity.size);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@obuscated"), _entity.obuscated);
		}
	public:
		imported_trackInsertDbQuery(const LocalDB::DBContext& ctx, const imported_track& entity) : LocalDB::NoResultDBQuery(ctx), _entity(entity) {}
	};
	class imported_trackInsertOrReplaceDbQuery : public LocalDB::NoResultDBQuery {
	private:
		imported_track _entity;
	protected:
		virtual std::string identifier() override { return "gen-imported_track-insertorreplace"; }
		virtual std::string sql(int) override {
			return "insert or replace into imported_track(id, quality, last_playpack_time, album_id, album_title, cover, title, artist, json, import_timestamp, size, obuscated) values (@id, @quality, @last_playpack_time, @album_id, @album_title, @cover, @title, @artist, @json, @import_timestamp, @size, @obuscated)";
		}
		virtual  void bindParameters(int, sqlite3*, sqlite3_stmt* statement) override {
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@id"), _entity.id);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@quality"), _entity.quality);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@last_playpack_time"), _entity.last_playpack_time);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@album_id"), _entity.album_id);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@album_title"), _entity.album_title);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@cover"), _entity.cover);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@title"), _entity.title);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@artist"), _entity.artist);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@json"), _entity.json);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@import_timestamp"), _entity.import_timestamp);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@size"), _entity.size);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@obuscated"), _entity.obuscated);
		}
	public:
		imported_trackInsertOrReplaceDbQuery(const LocalDB::DBContext& ctx, const imported_track& entity) : LocalDB::NoResultDBQuery(ctx), _entity(entity) {}
	};
	class imported_trackUpdateDbQuery : public LocalDB::NoResultDBQuery {
	private:
		imported_track _entity;
	protected:
		virtual std::string identifier() override { return "gen-imported_track-update"; }
		virtual std::string sql(int) override {
			return "update imported_track SET quality = @quality, last_playpack_time = @last_playpack_time, album_id = @album_id, album_title = @album_title, cover = @cover, title = @title, artist = @artist, json = @json, import_timestamp = @import_timestamp, size = @size, obuscated = @obuscated WHERE id = @id";
		}
		virtual  void bindParameters(int, sqlite3*, sqlite3_stmt* statement) override {
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@id"), _entity.id);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@quality"), _entity.quality);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@last_playpack_time"), _entity.last_playpack_time);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@album_id"), _entity.album_id);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@album_title"), _entity.album_title);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@cover"), _entity.cover);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@title"), _entity.title);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@artist"), _entity.artist);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@json"), _entity.json);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@import_timestamp"), _entity.import_timestamp);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@size"), _entity.size);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@obuscated"), _entity.obuscated);
		}
	public:
		imported_trackUpdateDbQuery(const LocalDB::DBContext& ctx, const imported_track& entity) : LocalDB::NoResultDBQuery(ctx), _entity(entity) {}
	};
	struct imported_album {
		std::int64_t id;
		std::wstring cover;
		std::wstring title;
		std::wstring artist;
		std::wstring json;
		std::wstring tracks_json;
		std::int64_t import_timestamp;
		static std::string getOrderedColumnsForSelect() {
			return "id, cover, title, artist, json, tracks_json, import_timestamp";
		}
		static imported_album createFromSqlRecord(sqlite3_stmt* stmt) {
			imported_album data;
			data.id = sqlite3_column_int64(stmt, 0);
			data.cover = LocalDB::sqlite3_column_wstring(stmt, 1);
			data.title = LocalDB::sqlite3_column_wstring(stmt, 2);
			data.artist = LocalDB::sqlite3_column_wstring(stmt, 3);
			data.json = LocalDB::sqlite3_column_wstring(stmt, 4);
			data.tracks_json = LocalDB::sqlite3_column_wstring(stmt, 5);
			data.import_timestamp = sqlite3_column_int64(stmt, 6);
			return data;
		}
	};
	class imported_albumInsertDbQuery : public LocalDB::NoResultDBQuery {
	private:
		imported_album _entity;
	protected:
		virtual std::string identifier() override { return "gen-imported_album-insert"; }
		virtual std::string sql(int) override {
			return "insert into imported_album(id, cover, title, artist, json, tracks_json, import_timestamp) values (@id, @cover, @title, @artist, @json, @tracks_json, @import_timestamp)";
		}
		virtual  void bindParameters(int, sqlite3*, sqlite3_stmt* statement) override {
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@id"), _entity.id);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@cover"), _entity.cover);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@title"), _entity.title);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@artist"), _entity.artist);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@json"), _entity.json);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@tracks_json"), _entity.tracks_json);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@import_timestamp"), _entity.import_timestamp);
		}
	public:
		imported_albumInsertDbQuery(const LocalDB::DBContext& ctx, const imported_album& entity) : LocalDB::NoResultDBQuery(ctx), _entity(entity) {}
	};
	class imported_albumInsertOrReplaceDbQuery : public LocalDB::NoResultDBQuery {
	private:
		imported_album _entity;
	protected:
		virtual std::string identifier() override { return "gen-imported_album-insertorreplace"; }
		virtual std::string sql(int) override {
			return "insert or replace into imported_album(id, cover, title, artist, json, tracks_json, import_timestamp) values (@id, @cover, @title, @artist, @json, @tracks_json, @import_timestamp)";
		}
		virtual  void bindParameters(int, sqlite3*, sqlite3_stmt* statement) override {
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@id"), _entity.id);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@cover"), _entity.cover);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@title"), _entity.title);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@artist"), _entity.artist);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@json"), _entity.json);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@tracks_json"), _entity.tracks_json);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@import_timestamp"), _entity.import_timestamp);
		}
	public:
		imported_albumInsertOrReplaceDbQuery(const LocalDB::DBContext& ctx, const imported_album& entity) : LocalDB::NoResultDBQuery(ctx), _entity(entity) {}
	};
	class imported_albumUpdateDbQuery : public LocalDB::NoResultDBQuery {
	private:
		imported_album _entity;
	protected:
		virtual std::string identifier() override { return "gen-imported_album-update"; }
		virtual std::string sql(int) override {
			return "update imported_album SET cover = @cover, title = @title, artist = @artist, json = @json, tracks_json = @tracks_json, import_timestamp = @import_timestamp WHERE id = @id";
		}
		virtual  void bindParameters(int, sqlite3*, sqlite3_stmt* statement) override {
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@id"), _entity.id);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@cover"), _entity.cover);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@title"), _entity.title);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@artist"), _entity.artist);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@json"), _entity.json);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@tracks_json"), _entity.tracks_json);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@import_timestamp"), _entity.import_timestamp);
		}
	public:
		imported_albumUpdateDbQuery(const LocalDB::DBContext& ctx, const imported_album& entity) : LocalDB::NoResultDBQuery(ctx), _entity(entity) {}
	};
	struct imported_playlist {
		std::wstring id;
		std::wstring cover;
		std::wstring title;
		std::wstring json;
		std::wstring tracks_json;
		std::int64_t updated_at;
		std::int64_t import_timestamp;
		static std::string getOrderedColumnsForSelect() {
			return "id, cover, title, json, tracks_json, updated_at, import_timestamp";
		}
		static imported_playlist createFromSqlRecord(sqlite3_stmt* stmt) {
			imported_playlist data;
			data.id = LocalDB::sqlite3_column_wstring(stmt, 0);
			data.cover = LocalDB::sqlite3_column_wstring(stmt, 1);
			data.title = LocalDB::sqlite3_column_wstring(stmt, 2);
			data.json = LocalDB::sqlite3_column_wstring(stmt, 3);
			data.tracks_json = LocalDB::sqlite3_column_wstring(stmt, 4);
			data.updated_at = sqlite3_column_int64(stmt, 5);
			data.import_timestamp = sqlite3_column_int64(stmt, 6);
			return data;
		}
	};
	class imported_playlistInsertDbQuery : public LocalDB::NoResultDBQuery {
	private:
		imported_playlist _entity;
	protected:
		virtual std::string identifier() override { return "gen-imported_playlist-insert"; }
		virtual std::string sql(int) override {
			return "insert into imported_playlist(id, cover, title, json, tracks_json, updated_at, import_timestamp) values (@id, @cover, @title, @json, @tracks_json, @updated_at, @import_timestamp)";
		}
		virtual  void bindParameters(int, sqlite3*, sqlite3_stmt* statement) override {
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@id"), _entity.id);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@cover"), _entity.cover);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@title"), _entity.title);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@json"), _entity.json);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@tracks_json"), _entity.tracks_json);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@updated_at"), _entity.updated_at);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@import_timestamp"), _entity.import_timestamp);
		}
	public:
		imported_playlistInsertDbQuery(const LocalDB::DBContext& ctx, const imported_playlist& entity) : LocalDB::NoResultDBQuery(ctx), _entity(entity) {}
	};
	class imported_playlistInsertOrReplaceDbQuery : public LocalDB::NoResultDBQuery {
	private:
		imported_playlist _entity;
	protected:
		virtual std::string identifier() override { return "gen-imported_playlist-insertorreplace"; }
		virtual std::string sql(int) override {
			return "insert or replace into imported_playlist(id, cover, title, json, tracks_json, updated_at, import_timestamp) values (@id, @cover, @title, @json, @tracks_json, @updated_at, @import_timestamp)";
		}
		virtual  void bindParameters(int, sqlite3*, sqlite3_stmt* statement) override {
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@id"), _entity.id);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@cover"), _entity.cover);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@title"), _entity.title);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@json"), _entity.json);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@tracks_json"), _entity.tracks_json);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@updated_at"), _entity.updated_at);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@import_timestamp"), _entity.import_timestamp);
		}
	public:
		imported_playlistInsertOrReplaceDbQuery(const LocalDB::DBContext& ctx, const imported_playlist& entity) : LocalDB::NoResultDBQuery(ctx), _entity(entity) {}
	};
	class imported_playlistUpdateDbQuery : public LocalDB::NoResultDBQuery {
	private:
		imported_playlist _entity;
	protected:
		virtual std::string identifier() override { return "gen-imported_playlist-update"; }
		virtual std::string sql(int) override {
			return "update imported_playlist SET cover = @cover, title = @title, json = @json, tracks_json = @tracks_json, updated_at = @updated_at, import_timestamp = @import_timestamp WHERE id = @id";
		}
		virtual  void bindParameters(int, sqlite3*, sqlite3_stmt* statement) override {
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@id"), _entity.id);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@cover"), _entity.cover);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@title"), _entity.title);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@json"), _entity.json);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@tracks_json"), _entity.tracks_json);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@updated_at"), _entity.updated_at);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@import_timestamp"), _entity.import_timestamp);
		}
	public:
		imported_playlistUpdateDbQuery(const LocalDB::DBContext& ctx, const imported_playlist& entity) : LocalDB::NoResultDBQuery(ctx), _entity(entity) {}
	};
	struct track_import_job {
		std::int64_t id;
		std::int64_t server_timestamp;
		std::int64_t server_size;
		std::int64_t local_size;
		std::wstring cover;
		std::wstring title;
		std::wstring artist;
		std::int64_t import_timestamp;
		std::int64_t owning_albumid;
		std::wstring owning_playlistid;
		std::int64_t quality;
		std::int64_t obuscated;
		static std::string getOrderedColumnsForSelect() {
			return "id, server_timestamp, server_size, local_size, cover, title, artist, import_timestamp, owning_albumid, owning_playlistid, quality, obuscated";
		}
		static track_import_job createFromSqlRecord(sqlite3_stmt* stmt) {
			track_import_job data;
			data.id = sqlite3_column_int64(stmt, 0);
			data.server_timestamp = sqlite3_column_int64(stmt, 1);
			data.server_size = sqlite3_column_int64(stmt, 2);
			data.local_size = sqlite3_column_int64(stmt, 3);
			data.cover = LocalDB::sqlite3_column_wstring(stmt, 4);
			data.title = LocalDB::sqlite3_column_wstring(stmt, 5);
			data.artist = LocalDB::sqlite3_column_wstring(stmt, 6);
			data.import_timestamp = sqlite3_column_int64(stmt, 7);
			data.owning_albumid = sqlite3_column_int64(stmt, 8);
			data.owning_playlistid = LocalDB::sqlite3_column_wstring(stmt, 9);
			data.quality = sqlite3_column_int64(stmt, 10);
			data.obuscated = sqlite3_column_int64(stmt, 11);
			return data;
		}
	};
	class track_import_jobInsertDbQuery : public LocalDB::NoResultDBQuery {
	private:
		track_import_job _entity;
	protected:
		virtual std::string identifier() override { return "gen-track_import_job-insert"; }
		virtual std::string sql(int) override {
			return "insert into track_import_job(id, server_timestamp, server_size, local_size, cover, title, artist, import_timestamp, owning_albumid, owning_playlistid, quality, obuscated) values (@id, @server_timestamp, @server_size, @local_size, @cover, @title, @artist, @import_timestamp, @owning_albumid, @owning_playlistid, @quality, @obuscated)";
		}
		virtual  void bindParameters(int, sqlite3*, sqlite3_stmt* statement) override {
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@id"), _entity.id);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@server_timestamp"), _entity.server_timestamp);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@server_size"), _entity.server_size);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@local_size"), _entity.local_size);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@cover"), _entity.cover);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@title"), _entity.title);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@artist"), _entity.artist);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@import_timestamp"), _entity.import_timestamp);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@owning_albumid"), _entity.owning_albumid);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@owning_playlistid"), _entity.owning_playlistid);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@quality"), _entity.quality);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@obuscated"), _entity.obuscated);
		}
	public:
		track_import_jobInsertDbQuery(const LocalDB::DBContext& ctx, const track_import_job& entity) : LocalDB::NoResultDBQuery(ctx), _entity(entity) {}
	};
	class track_import_jobInsertOrReplaceDbQuery : public LocalDB::NoResultDBQuery {
	private:
		track_import_job _entity;
	protected:
		virtual std::string identifier() override { return "gen-track_import_job-insertorreplace"; }
		virtual std::string sql(int) override {
			return "insert or replace into track_import_job(id, server_timestamp, server_size, local_size, cover, title, artist, import_timestamp, owning_albumid, owning_playlistid, quality, obuscated) values (@id, @server_timestamp, @server_size, @local_size, @cover, @title, @artist, @import_timestamp, @owning_albumid, @owning_playlistid, @quality, @obuscated)";
		}
		virtual  void bindParameters(int, sqlite3*, sqlite3_stmt* statement) override {
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@id"), _entity.id);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@server_timestamp"), _entity.server_timestamp);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@server_size"), _entity.server_size);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@local_size"), _entity.local_size);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@cover"), _entity.cover);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@title"), _entity.title);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@artist"), _entity.artist);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@import_timestamp"), _entity.import_timestamp);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@owning_albumid"), _entity.owning_albumid);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@owning_playlistid"), _entity.owning_playlistid);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@quality"), _entity.quality);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@obuscated"), _entity.obuscated);
		}
	public:
		track_import_jobInsertOrReplaceDbQuery(const LocalDB::DBContext& ctx, const track_import_job& entity) : LocalDB::NoResultDBQuery(ctx), _entity(entity) {}
	};
	class track_import_jobUpdateDbQuery : public LocalDB::NoResultDBQuery {
	private:
		track_import_job _entity;
	protected:
		virtual std::string identifier() override { return "gen-track_import_job-update"; }
		virtual std::string sql(int) override {
			return "update track_import_job SET server_timestamp = @server_timestamp, server_size = @server_size, local_size = @local_size, cover = @cover, title = @title, artist = @artist, import_timestamp = @import_timestamp, owning_albumid = @owning_albumid, owning_playlistid = @owning_playlistid, quality = @quality, obuscated = @obuscated WHERE id = @id";
		}
		virtual  void bindParameters(int, sqlite3*, sqlite3_stmt* statement) override {
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@id"), _entity.id);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@server_timestamp"), _entity.server_timestamp);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@server_size"), _entity.server_size);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@local_size"), _entity.local_size);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@cover"), _entity.cover);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@title"), _entity.title);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@artist"), _entity.artist);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@import_timestamp"), _entity.import_timestamp);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@owning_albumid"), _entity.owning_albumid);
			LocalDB::sqlite3_bind_string(statement, sqlite3_bind_parameter_index(statement, "@owning_playlistid"), _entity.owning_playlistid);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@quality"), _entity.quality);
			sqlite3_bind_int64(statement, sqlite3_bind_parameter_index(statement, "@obuscated"), _entity.obuscated);
		}
	public:
		track_import_jobUpdateDbQuery(const LocalDB::DBContext& ctx, const track_import_job& entity) : LocalDB::NoResultDBQuery(ctx), _entity(entity) {}
	};
}
