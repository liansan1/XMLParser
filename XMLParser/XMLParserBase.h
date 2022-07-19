#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <regex>
#include <fstream>
#include <sstream>
#include <stack>
#include <map>


/*xml�ĵ������ṹ�ı�ʾ*/
namespace XMLParser {
	
	typedef std::string PropertyKey;
	typedef std::string PropertyVal;
	typedef std::string ElementName;  //Ԫ����
	/*��ʾXML���ԵĽṹ��*/
	struct XMLProperty {
		PropertyKey name;   //������
		PropertyVal text;   //����ֵ
	};

	/*��ʾXMLԪ�صĽṹ��,һ��Ԫ�ذ���һ����ǩ��һ�����ԣ������Ԫ�غ�
	�����ɸ���Ԫ�أ������Щ��Ԫ�ص���һ����Ԫ��������XMLProperty��*/
	typedef std::map<ElementName, XMLProperty> XMLElement;

	typedef std::multimap<std::string, std::string> MemberShipArray;
	/*��ʾXML�ڵ�Ľṹ��*/
	struct XMLNode {
	public:
		void ShowNode(XMLNode *root);
		void CreateNode(XMLNode **node, XMLElement element);
		void InintNode(XMLNode **root, XMLElement element);
		XMLNode* DestoryTree(XMLNode *root);
	public:
		XMLElement Element; //�ڵ�����
		XMLNode* firstchild; //ָ���һ�����ӽڵ��ָ��
		XMLNode* nextlibling;  //ָ����һ�����ӽڵ��ָ��
	};

	/*��ȡXML�ļ����ڴ棬������*/
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
		std::string XMLCache; //xml�ĵ��Ļ���
		std::string XMLRoot;  //���ڵ��ǩ��
		XMLNode* root;        //���ڵ�ָ��
		std::multimap<std::string, std::string> ParentInfo; //��¼xml�и��ӽڵ��ϵ
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
