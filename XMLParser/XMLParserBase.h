#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <regex>
#include <fstream>
#include <sstream>
#include <stack>
#include <map>


/*xml文档各个结构的表示*/
namespace XMLParser {
	
	typedef std::string PropertyKey;
	typedef std::string PropertyVal;
	typedef std::string ElementName;  //元素名
	/*表示XML属性的结构体*/
	struct XMLProperty {
		PropertyKey name;   //属性名
		PropertyVal text;   //属性值
	};

	/*表示XML元素的结构体,一个元素包含一个标签，一个属性，如果该元素含
	有若干个子元素，则把这些子元素当成一个大元素整体存进XMLProperty中*/
	typedef std::map<ElementName, XMLProperty> XMLElement;

	typedef std::multimap<std::string, std::string> MemberShipArray;
	/*表示XML节点的结构体*/
	struct XMLNode {
	public:
		void ShowNode(XMLNode *root);
		void CreateNode(XMLNode **node, XMLElement element);
		void InintNode(XMLNode **root, XMLElement element);
		XMLNode* DestoryTree(XMLNode *root);
	public:
		XMLElement Element; //节点数据
		XMLNode* firstchild; //指向第一个孩子节点的指针
		XMLNode* nextlibling;  //指向下一个孩子节点的指针
	};

	/*读取XML文件到内存，并解析*/
	class XMLReader {
	public:

		XMLNode* XMLFileLoad(const std::string FileName);
	protected:
		void GenerateXMLTree(XMLNode** node, std::string nodename);

		bool ParserNode(const std::string xmlstr);

		std::string CreateEndTag(const std::string startTag);
	
		XMLElement GetElementContent(const std::string XMLCache, const std::string ElementName);
		bool ParseElement(std::string);

		std::string GetElementName(std::string ElementStr);

		bool JudgeElemOrText(std::string XMLCache);

		bool CheckXML();

		bool CheckLable(std::string strXml,bool* pbIsSymmetry);

		bool ParserProperty(std::string, XMLProperty &Property);
	private:
		std::string XMLCache; //xml文档的缓存
		std::string XMLRoot;  //根节点标签名
		XMLNode* root;        //根节点指针
		std::multimap<std::string, std::string> ParentInfo; //记录xml中父子节点关系
	};
}


int main()
{
	XMLParser::XMLReader reader;
	XMLParser::XMLNode* test = reader.XMLFileLoad("D:\\admin.xml");
	test->ShowNode(test);
	XMLParser::XMLNode* destory = test->DestoryTree(test);
	
	destory->ShowNode(destory);
}
