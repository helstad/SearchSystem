#include "link_extractor.h"

#include <algorithm>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

std::vector<std::string>
link_extractor::extract_links( const std::string& html_content,
	const std::string& base_url ) {
	std::vector<std::string> links;
	std::regex pattern( "<a\\s+(?:[^>]*?\\s+)?href=\"([^\"]*)\"" );

	auto links_begin =
		std::sregex_iterator( html_content.begin(), html_content.end(), pattern );
	auto links_end = std::sregex_iterator();

	for ( std::sregex_iterator i = links_begin; i != links_end; ++i ) {
		std::string link = ( *i )[ 1 ].str();

		if ( link.starts_with( "#" ) ) {
			continue;
		}

		link = remove_fragment( link );

		std::string full_url = normalize_url( base_url, link );

		if ( !full_url.empty() && is_valid_url( full_url ) ) {
				links.push_back( full_url );
		}
	}

	return links;
}

std::string link_extractor::remove_fragment( const std::string& url ) {
	size_t pos = url.find( '#' );
	return ( pos != std::string::npos ) ? url.substr( 0, pos ) : url;
}

std::string link_extractor::normalize_url( const std::string& base_url,
	const std::string& relative_url ) {
	if ( relative_url.starts_with( "http" ) ) { return normalize_absolute_url( relative_url ); }

	std::string normalized_url = relative_url;
	std::replace( normalized_url.begin(), normalized_url.end(), '\\', '/' );

	if ( normalized_url.starts_with( "//" ) ) {
		size_t protocol_pos = base_url.find( "://" );
		std::string protocol = ( protocol_pos != std::string::npos )
			? base_url.substr( 0, protocol_pos )
			: "http";

		return normalize_absolute_url( protocol + ": " + normalized_url );
	}

	if ( normalized_url.starts_with( "/" ) ) {
		size_t domain_end = base_url.find( '/', base_url.find( "://" ) + 3 );
		std::string domain = ( domain_end != std::string::npos )
			? base_url.substr( 0, domain_end )
			: base_url;

		return normalize_absolute_url( domain + normalized_url );
	}

	size_t last_slash_pos = base_url.rfind( '/' );
	std::string base = ( last_slash_pos != std::string::npos )
		? base_url.substr( 0, last_slash_pos + 1 )
		: base_url + "/";

	return normalize_absolute_url( base + normalized_url );
}

std::string link_extractor::normalize_absolute_url( const std::string& url ) {
	std::string normalized_url = url;
	size_t protocol_pos = normalized_url.find( "://" );

	if ( protocol_pos != std::string::npos ) {
		std::transform( normalized_url.begin(), normalized_url.begin() + protocol_pos + 3,
			normalized_url.begin(), ::tolower );
	}

	if ( normalized_url.back() == '/' ) { normalized_url.pop_back(); }

	size_t pos = normalized_url.find( "://www." );
	if ( pos != std::string::npos ) {
		normalized_url.replace( pos + 3, 4, "" );
	}

	return normalized_url;
}

bool link_extractor::is_valid_url( const std::string& url ) {
	static const std::unordered_set<char> invalid_chars { '(', ')', '?', ';' };
	if ( std::any_of( url.begin(), url.end(),
		[ & ] ( char c ) { return invalid_chars.count( c ); } ) )
		return false;

	static const std::unordered_map<std::string, bool> blacklist {
			{"?action=", true}, {"?p=", true}, {";a=", true} };

	for ( const auto& [key, value] : blacklist ) {
		if ( url.find( key ) != std::string::npos )
			return false;
	}

	if ( url.find( ':' ) != std::string::npos &&
		url.find( "://" ) != std::string::npos ) {
		size_t protocol_end_pos = url.find( "://" );
		std::string path = url.substr( protocol_end_pos + 3 );
		if ( path.find( ':' ) != std::string::npos ) {
			return false;
		}
	}

	static const std::unordered_set<std::string> suffix_blacklist {
			"/wiki/",    "/Category:", "/File:",   "/Special:", "/User:",
			"_missing_", "index.php?", "mailto:",  ".pdf",      ".jpg",
			".png",      ".gif",       ".css",     ".js",       ".ico",
			".svg",      ".xml",       "api.php?", "gitweb" };

	for ( const auto& suffix : suffix_blacklist ) {
		if ( url.find( suffix ) != std::string::npos ) { return false; }
	}

	return true;
}
