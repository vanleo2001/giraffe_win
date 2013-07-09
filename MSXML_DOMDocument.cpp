#include "stdafx.h"
#include <atlbase.h>
#include <atlconv.h>

#include "MSXML_DOMDocument.h"
#include <sstream>

namespace STEP
{
  MSXML_DOMAttributes::~MSXML_DOMAttributes()
  { 
    if(m_pNodeMap) m_pNodeMap->Release();
    
  }

  bool MSXML_DOMAttributes::get( const std::string& name, std::string& value )
  { 

    if(!m_pNodeMap) return false;
    MSXML2::IXMLDOMNode* pNode = NULL;
    m_pNodeMap->getNamedItem(_bstr_t(name.c_str()), &pNode);
    if( pNode == NULL ) return false;

    BSTR result;
    pNode->get_text(&result);
    value = (char*)_bstr_t(result);
    pNode->Release();
    return true;
  
  }

  DOMAttributes::map MSXML_DOMAttributes::toMap()
  {
    return DOMAttributes::map();
  }

  MSXML_DOMNode::~MSXML_DOMNode()
  { 
    m_pNode->Release();
  }

  DOMNodePtr MSXML_DOMNode::getFirstChildNode()
  { 
    MSXML2::IXMLDOMNode* pNode = NULL;
    m_pNode->get_firstChild(&pNode);
    if( pNode == NULL ) return DOMNodePtr();
    return DOMNodePtr(new MSXML_DOMNode(pNode));
  }

  DOMNodePtr MSXML_DOMNode::getNextSiblingNode()
  { 
    MSXML2::IXMLDOMNode* pNode = NULL;
    m_pNode->get_nextSibling(&pNode);
    if( pNode == NULL ) return DOMNodePtr();
    return DOMNodePtr(new MSXML_DOMNode(pNode));
  }

  DOMAttributesPtr MSXML_DOMNode::getAttributes()
  { 
    return DOMAttributesPtr(new MSXML_DOMAttributes(m_pNode));
    
  }

  std::string MSXML_DOMNode::getName()
  { 
    BSTR result;
    m_pNode->get_nodeName(&result);
    return (char*)_bstr_t(result);
    
  }

  std::string MSXML_DOMNode::getText()
  { 
    BSTR result;
    m_pNode->get_text(&result);
    return (char*)_bstr_t(result);
    
  }

  MSXML_DOMDocument::MSXML_DOMDocument() throw( ConfigError )
  : m_pDoc(NULL)
  {
    if(FAILED(CoInitializeEx( 0, COINIT_MULTITHREADED )))
      if(FAILED(CoInitializeEx( 0, COINIT_APARTMENTTHREADED )))
        throw ConfigError("Could not initialize COM");

    HRESULT hr = CoCreateInstance(
      MSXML2::CLSID_DOMDocument, NULL, CLSCTX_ALL, __uuidof( MSXML2::IXMLDOMDocument2 ),
      ( void ** ) & m_pDoc );

    if ( hr != S_OK )
      throw( ConfigError( "MSXML DOM Document could not be created" ) );
  }

  MSXML_DOMDocument::~MSXML_DOMDocument()
  { 

    if(m_pDoc != NULL)
      m_pDoc->Release();
    CoUninitialize();
    
  }

  bool MSXML_DOMDocument::load( std::istream& stream )
  { 
    try
    {
      m_pDoc->put_async(VARIANT_FALSE);
      m_pDoc->put_resolveExternals(VARIANT_FALSE);
      m_pDoc->setProperty(_bstr_t("SelectionLanguage"), _variant_t("XPath"));

      std::stringstream sstream;
      sstream << stream.rdbuf();

      VARIANT_BOOL success = FALSE;
      m_pDoc->loadXML(_bstr_t(sstream.str().c_str()), &success);
      return success != FALSE;
    }
    catch( ... ) { return false; }
    
  }

  bool MSXML_DOMDocument::load( const std::string& url )
  { 
    try
    {
      m_pDoc->put_async(VARIANT_FALSE);
      m_pDoc->put_resolveExternals(VARIANT_FALSE);
      m_pDoc->setProperty(_bstr_t("SelectionLanguage"), _variant_t("XPath"));

      VARIANT_BOOL success = FALSE;
      m_pDoc->load(_variant_t(url.c_str()), &success);
      return success != FALSE;
    }
    catch( ... ) { return false; }
    
  }

  bool MSXML_DOMDocument::xml( std::ostream& out )
  { 
    try
    {
      BSTR result;
      HRESULT hr = m_pDoc->get_xml(&result);
      if( hr != S_OK ) return false;
      out << (char*)_bstr_t(result);
      return true;
    }
    catch( ... ) { return false; }
    
  }

  DOMNodePtr MSXML_DOMDocument::getNode( const std::string& XPath )
  {
    HRESULT hr;

    MSXML2::IXMLDOMNode* pNode = NULL;
    hr = m_pDoc->selectSingleNode(_bstr_t(XPath.c_str()), &pNode);
    if( pNode == NULL ) return DOMNodePtr();
    return DOMNodePtr(new MSXML_DOMNode(pNode));
    
  }
}


