#include "xml2cfg.h"
#include <map>
#include <string>

typedef std::map< std::string, int > map_typesize_t;
map_typesize_t did_type_map;

int cat_to( char *dest, const char *val, int pos )
{
	int _len = strlen( val );
	strcat( dest + pos, val );
	return pos + _len;
}

void auto_did_size()
{
	/*! generate by vim marco. */
	did_type_map.insert( std::make_pair("repeat", 4) ); 
	did_type_map.insert( std::make_pair("char", 1) ); 
	did_type_map.insert( std::make_pair("int16", 2) ); 
	did_type_map.insert( std::make_pair("int32", 4) ); 
	did_type_map.insert( std::make_pair("int64", 8) ); 
	did_type_map.insert( std::make_pair("byte", 1) ); 
	did_type_map.insert( std::make_pair("uint16", 2) ); 
	did_type_map.insert( std::make_pair("uint24", 3) ); 
	did_type_map.insert( std::make_pair("uint32", 4) ); 
	did_type_map.insert( std::make_pair("uint64", 8) ); 
	did_type_map.insert( std::make_pair("pfloat", 8) ); 
	did_type_map.insert( std::make_pair("dzh_time_t", 8) ); 
	did_type_map.insert( std::make_pair("vfloat", 8) );
}

//int main( int argc, char **argv )
//{
//	xml2cfg _cfg;
//	_cfg.open("100000.xml");
//	xmlNodeSetPtr _info_set = _cfg.locate( NULL, "/datas/data[@id=100000]");
//	xmlNodePtr _info = _cfg.get_node( _info_set, 0 );
//	/*! read version.*/
//	int _ver = _cfg.get_prop_int( _info, "drvver", 0 );
//	if( 0 == _ver )
//		return 0;
//
//	xmlNodeSetPtr _did_set = _cfg.locate( _info, "dids/did[@id=100000]");
//	if( _cfg.get_nodes_num( _did_set ) != 1 )
//		return 0;
//
//	xmlNodePtr _desc = _cfg.get_node( _did_set, 0 );
//	char _name[128]  = {0};
//	char _cname[128] = {0};
//	_cfg.get_prop_string( _desc, "name", _name, NULL );
//	_cfg.get_prop_string( _desc, "des", _cname, NULL );
//	
//	xmlNodeSetPtr _fields = _cfg.locate( _info, "fields/*");
//	int _num = _cfg.get_nodes_num( _fields );
//	char _lua_cdef[2048] = {0};
//	int _pos = 0;
//
//	_pos = cat_to( _lua_cdef, "\nffi.cdef[[\n\tstruct ", _pos );
//	std::string _head = std::string(_name) + "--" + _cname + "\n{\n";
//	_pos = cat_to( _lua_cdef, _head.c_str(), _pos );
//
//	map_typesize_t::iterator _iter;
//	for( int i = 0; i < _num; ++i )
//	{
//		xmlNodePtr _field = _cfg.get_node( _fields, i );
//		char _commet[128] = {0};
//		char _field_name[128] = {0};
//		_cfg.get_prop_string( _field, "name", _field_name, NULL );
//		_cfg.get_prop_string( _field, "des", _commet, NULL );
//		std::string _line = std::string( (const char *)_field->name ) +
//			"\t"+
//			std::string(_field_name)+
//			";--"+
//			_commet+
//			"\n";
//
//		_pos = cat_to( _lua_cdef, _line.c_str(), _pos );
//		int _bk = 0;
//	}
//	_pos = cat_to( _lua_cdef, "}\n]];\n", _pos );
//
//	FILE *_fp = fopen( "test.lua", "wb" );
//	if( _fp	 != NULL )
//	{
//		fwrite( _lua_cdef, 1, _pos, _fp );
//		fclose( _fp );
//	}
//
//	return 0;
//}
