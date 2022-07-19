#include "XMLParserBase.h"

using namespace XMLParser;

/*��ʼ�����ڵ�*/
void XMLNode::InintNode(XMLNode **root,XMLElement element)
{
	*root = new(XMLNode);
	(*root)->firstchild = nullptr;
	(*root)->nextlibling  = nullptr;
	(*root)->Element = element;
}

/*����һ���µĽڵ�*/
void XMLNode::CreateNode(XMLNode **node, XMLElement element)
{
	*node = new(XMLNode);
	(*node)->firstchild = nullptr;
	(*node)->nextlibling = nullptr;
	(*node)->Element = element;
}

/*�������������ʾ*/
void XMLNode::ShowNode(XMLNode *node)
{
	/*��ӡxml������*/
	if (node)
	{
		for (auto it = node->Element.begin(); it != node->Element.end(); it++)
		{
			std::cout << "ElementName: " << it->first << " PropertyName: " << it->second.name << std::endl;
			std::cout << "***************************PropertyText***************************" << std::endl;
			std::cout << it->second.text << std::endl;
			std::cout << "******************************************************************" << std::endl;
		}
		if (node->firstchild)
		{
			XMLNode* temp = node->firstchild;
			ShowNode(temp);
			temp = temp->nextlibling;
			while (temp)
			{
				ShowNode(temp);
				temp = temp->nextlibling;
			}
		}
	}
	
}

XMLNode* XMLNode::DestoryTree(XMLNode *root)
{
	if(root == nullptr)
	{
		return NULL;
	}
	else
	{
		XMLNode* tempfir = root->firstchild;
		XMLNode* tempsib;
		while (tempfir)
		{
			tempsib = tempfir->nextlibling;
			DestoryTree(tempfir);
			tempfir = tempsib;
		}
		delete root;
		return nullptr;
	}
}

/*ʹ�ú����ֵܱ�ʾ������һ��������洢�ڵ�����*/
void XMLReader::GenerateXMLTree(XMLNode** node,std::string nodename)
{
	XMLNode* temp;
	XMLNode* move = (*node);
	auto  count = ParentInfo.count(nodename);
	//���Ҵ���ڵ�����к��ӽڵ�
	auto  iter = ParentInfo.find(nodename);
	
	for (int i = 0; i < count; i++)
	{
		if (i == 0)
		{
			temp->CreateNode(&temp, GetElementContent(XMLCache, iter->second));
			move->firstchild = temp;
			move = temp;
			GenerateXMLTree(&move,iter->second); //��һ���ڵ��Ǵ���ڵ���ӽڵ�
		}
		else
		{
			temp->CreateNode(&temp, GetElementContent(XMLCache, iter->second));
			move->nextlibling = temp;
			move = temp;
			GenerateXMLTree(&move, iter->second);//֮�����еĽڵ㶼�ǵ�һ���ڵ���ֵܽڵ�
		}
		iter++;
	}
}

/*��ȡ�ļ����ݵ��ڴ�*/
XMLNode* XMLReader::XMLFileLoad(const std::string FileName)
{
	//�����ļ����ݵ��ڴ�
	std::ifstream read(FileName,std::ios::binary);
	std::stringstream stream;
	stream << read.rdbuf();
	XMLCache = stream.str();
	if (!CheckXML())
	{
		exit(-1);
	}

	//����xml����
	ParserNode(XMLCache);

	//��ʼ�����ڵ�,����XML�����
	root->InintNode(&root, GetElementContent(XMLCache, XMLRoot));
	GenerateXMLTree(&root, XMLRoot);
	return root;
}


/*������һ�����ڵ㲢���������*/
bool XMLReader::ParserNode(const std::string xmlstr)
{
	//XML�ļ�ֻ��һ����Ԫ�أ�������ǩ����ȡ���������ĸ��ڵ�
	std::smatch m;
	std::string RootArray;
	std::regex e("<(\\w+).*>|<\\w+[^/>]*/>");
	if (std::regex_search(xmlstr, m, e))  //���ƥ�䵽<> ... </>
	{
		std::string temp = m.str();
		XMLRoot = GetElementName(temp);
	}
	ParseElement(xmlstr);
	return true;
}


/*������ʼ��ǩ�Ƶ���������ǩ <***> -> </***> */
std::string XMLReader::CreateEndTag(const std::string startTag)
{
	std::string EndTag;
	char EndFlag = '/';

	std::smatch m;
	std::regex e("<\\w+\\b");
	std::regex_search(startTag, m, e); //ƥ���ǩ��
	EndTag = m.str();
	EndTag.insert(1, 1, EndFlag);
	EndTag += '>';
	return EndTag;
}

/*��ȡ��ǩ���ڵ���������*/
XMLElement XMLReader::GetElementContent(const std::string XMLCache, const std::string ElementName)
{
	XMLElement temp;
	XMLProperty property;
	std::string content;
	std::string name = ElementName;
	std::string starttag = '<' + ElementName + '>';
	std::string endtag = CreateEndTag(starttag);
	int offset = XMLCache.find(endtag);
	if (offset > 0)    //�ҵ��˽�����ǩ����ȡ����ʼ������������
	{
		if (ElementName == XMLRoot)
		{
			content = XMLCache.substr(ElementName.length() + XMLCache.find(ElementName) + 1, XMLCache.find(endtag)-endtag.length());
			content = content.substr(0, content.find(endtag));
		}
		else 
		{
			//��ȡһ����������������
			content = XMLCache.substr(ElementName.length() + XMLCache.find(ElementName) + 1, XMLCache.find(endtag));
			content = content.substr(0, content.find(endtag));
		}
		if (JudgeElemOrText(content)) //content���溬����Ԫ��
		{
			property.name = "haschild";
			property.text = content;
		}
		else
		{
			property.name = "text";
			property.text = content;
		}
	}
	else //û�ҵ�������ǩ��˵����< ... />
	{
		content = XMLCache.substr(XMLCache.find(ElementName) - 1, XMLCache.length());
		content = content.substr(0,content.find("/>"));
		//std::cout << "---------------" << content << "-----------------" << std::endl;
		ParserProperty(content,property);
	}
	temp.emplace(ElementName,property);
	return temp;
}

/*�жϸ����ڵ�ĸ��ӹ�ϵ�����洢������*/
bool XMLReader::ParseElement(std::string Element)
{
	//ͨ��ջ�ṹ�жϸ��ӽڵ�Ĺ�ϵ��ÿ����ǩԪ����ջ��������β��ǩ��ջ�����ĸ�Ԫ�ؾ���ջ��Ԫ�ء�
	std::string Search = Element;
	std::stack<std::string> LableStack;
	std::string tempstart;
	std::string tempend;
	std::smatch m;
	std::smatch h;

	//�ҵ���ʼ��ǩ
	std::regex e("<(\\w+).*>|<\\w+[^/>]*/>|</\\w+>");
	LableStack.push(XMLRoot);
	tempstart = XMLRoot;
	std::regex_search(Search, m, e);
	std::string startTag,endTag;
	startTag = m.str();
	endTag = CreateEndTag(startTag);
	Search = Search.substr(startTag.length() + Search.find(startTag), Search.find(endTag) + 1);
	Search = Search.substr(0, Search.find(endTag) - 1);

	while(std::regex_search(Search, m, e))  //���ƥ�䵽<> ... </>
	{
		tempstart = m.str();
		if (GetElementName(m.str()) == LableStack.top()) //���������ǩ��ջ��Ԫ����ͬ��˵����ǩ�Խ�����ջ��Ԫ�س�ջ
		{
			LableStack.pop();
			if (m.str().find("</") == m.str().npos)
			{
				if (LableStack.empty())
				{
					ParentInfo.emplace("NULL", GetElementName(m.str()));
				}
				else
				{
					ParentInfo.emplace(LableStack.top(), GetElementName(m.str()));
				}
			}
		}
		else  //���������ǩ��ջ��Ԫ�ز�ͬ��˵�������ӱ�ǩ����ջ
		{
			if (m.str().find("</") == m.str().npos)
			{
				tempend = "</" + GetElementName(m.str()) + '>'; //�ָ�������ǩ
				if (Element.find(tempend) != Element.npos) //����ҵ����н�����ǩ������<>...</>����
				{
					tempstart = GetElementName(m.str());
					ParentInfo.emplace(LableStack.top(), tempstart);
					LableStack.push(tempstart);
				}
				else   //���û�б�ǩ������< .../>����
				{
					tempstart = GetElementName(m.str());
					ParentInfo.emplace(LableStack.top(), tempstart);
				}
			}
		}
		Search = m.suffix().str();
	}
	return true;
}

/*��ȡ��ǩ��*/
std::string XMLReader::GetElementName(std::string ElementStr)
{
	std::string Name = "";
	std::string s = ElementStr;
	for (auto i : s)
	{
		if (i == ' ' || i == '>')
		{
			break;
		}
		else if (i == '<' || i== '/')
		{
			continue;
		}
		Name += i;
	}
	return Name;
}

/*�ж��Ƿ��������Ŀ������ֻ��һ��text��������*/
bool XMLReader::JudgeElemOrText(std::string ElementStr)
{
	std::string s = ElementStr;
	std::smatch m;
	std::regex regexItem("<(\\w+).*>.*</\\1>|<\\w+[^/>]*/>");
	// ƥ�䣺ʶ�����Ŀ
	if (std::regex_search(s, m, regexItem)) {
		return false;
	}
	// ƥ�䣺ʶ��� text ����
	else {
		return true;
	}
}

/*��������*/
bool XMLReader::ParserProperty(std::string PropertyStr,XMLProperty &Property)
{
	std::string Propertyname;
	std::string Propertyval;
	// ��ȡ����������
	std::string s = PropertyStr;
	std::smatch m;
	std::regex regexAtrributeName("\\b\\w+=");
	if (std::regex_search(s, m, regexAtrributeName)) {
		std::string strTempName = m.str();
		strTempName.erase(strTempName.end() - 1, strTempName.end());
		Propertyname = strTempName;
	}
	// ��ȡ������ֵ
	s = PropertyStr;
	std::regex regexAtrributeValue("\"\\w+\"");
	if (std::regex_search(s, m, regexAtrributeValue)) {
		std::string strTempValue = m.str();
		strTempValue.erase(strTempValue.end() - 1, strTempValue.end());
		strTempValue.erase(0, 1);
		Propertyval = strTempValue;
	}
	Property.name = Propertyname;
	Property.text = Propertyval;
	return true;
}

bool XMLReader::CheckXML()
{
	std::stack<char> stackBracket;		// ջ��������
	std::stack<char> stackBackSlash;	// ջ����б��
	std::stack<std::string> statckTab;  // ջ����ǩ��
	bool bBracketValid = true;			// �Ϸ���������ƥ��
	bool bBlackSlashValid = true;		// �Ϸ�����б��
	bool bIsSymmetry = true;			// �Ϸ�����ǩ�Գ���
	// ��飺����ƥ��
	for (int i = 0; i < XMLCache.size(); ++i) {
		// --��¼�������� < ����ֱ��ѹջ
		if ('<' == XMLCache[i]) stackBracket.push(XMLCache[i]);
		// --��¼�������� > �����ж�ջ�Ƿ�Ϊ�ա��Ƿ�ջ��Ϊ '<'�������������ջ
		else if ('>' == XMLCache[i] && stackBracket.size() != 0 && stackBracket.top() == '<')
			stackBracket.pop();
		// --��¼�������� / �����ж�ջ���Ƿ�Ϊ <������Ƿ��� < �����Ҳ��Ƿ��� >��
		// ��������ѹջ��������Ŀ�����������򱨴�
		else if ('/' == XMLCache[i] && stackBracket.top() == '<' &&
			((i != 0 && '<' == XMLCache[i - 1]) || (i + 1 != XMLCache.size() && '>' == XMLCache[i + 1])))
			stackBackSlash.push(XMLCache[i]);
		else if ('/' == XMLCache[i]) {
			bBlackSlashValid = false;
			break;
		}
	}
	// ������б�߲�ƥ��
	if (!bBlackSlashValid)
		std::cout << "parse error: black slash write error." << std::endl;
	// ���������Ų�ƥ��
	if (stackBracket.size() != 0) {
		bBracketValid = false;
		std::cout << "parse error: bracket does not matched." << std::endl;
	}
	// ������ǩ�Բ��Գ�
	CheckLable(XMLCache, &bIsSymmetry);
	if (!bIsSymmetry) {
		std::cout << "parse error: tab does not symmetry." << std::endl;
	}
	return bBlackSlashValid && bIsSymmetry && bBracketValid;
}

bool XMLReader::CheckLable(std::string strXml,bool *pbIsSymmetry)
{
	bool bIsSymmetry = true;
	std::string s = strXml;
	std::smatch m;
	std::regex regexTab("<\\w+[^\\n/<]*>|</\\w+[\\s]*>");
	std::regex regexHeadName("<\\w+\\b");
	std::regex regexTailName("/\\w+");
	std::stack<std::string> stackTab;
	// �ҵ�˫��ǩ�Եı�ǩͷ
	while (std::regex_search(s, m, regexTab)) {
		// --���:����ƥ����Ǳ�ǩͷ���Ǳ�ǩβ���в�ͬ�Ĵ���
		std::string strTemp = m.str();
		std::smatch tempMatch;
		// --ƥ�䣺��ǩͷ
		if (std::regex_search(strTemp, tempMatch, regexHeadName)) {
			std::string strTabName = tempMatch.str();
			strTabName.erase(0, 1);
			stackTab.push(strTabName);
		}
		// --ƥ�䣺��ǩβ
		else {
			if (std::regex_search(strTemp, tempMatch, regexTailName)) {
				std::string strTabName = tempMatch.str();
				strTabName.erase(0, 1);
				if (stackTab.size() != 0 && stackTab.top() == strTabName) {
					stackTab.pop();
				}
			}
		}
		s = m.suffix().str();
	}
	*pbIsSymmetry = stackTab.size() == 0 ? true : false;
	return true;
}