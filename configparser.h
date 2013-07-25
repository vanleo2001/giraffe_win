#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include "xml_class_set.h"
#include "xml2cfg.h"

class ConfigParser
{
public:
	ConfigParser(char * config_file):config_file_(config_file){};
	~ConfigParser(){};
	inline deque<XML_ListeningItem> & get_listeningitems()
	{
		return listeningitem_deque_;
	}
	inline XML_Log & get_log_obj()
	{
		return xml_log_;
	}
	inline vector<int> & get_ports()
	{
		return ports_;
	}
	void Parse();
protected:
private:
	std::string config_file_;
	vector<int> ports_;
	XML_Log xml_log_;
	deque<XML_ListeningItem> listeningitem_deque_;
};


#endif