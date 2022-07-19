#include "XMLParserBase.h"

using namespace XMLParser;

/*初始化根节点*/
void XMLNode::InintNode(XMLNode **root,XMLElement element)
{
	*root = new(XMLNode);
	(*root)->firstchild = nullptr;
	(*root)->nextlibling  = nullptr;
	(*root)->Element = element;
}

/*创建一个新的节点*/
void XMLNode::CreateNode(XMLNode **node, XMLElement element)
{
	*node = new(XMLNode);
	(*node)->firstchild = nullptr;
	(*node)->nextlibling = nullptr;
	(*node)->Element = element;
}

/*遍历多叉树并显示*/
void XMLNode::ShowNode(XMLNode *node)
{
	/*打印xml的数据*/
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

/*使用孩子兄弟表示法创建一个多叉树存储节点数据*/
void XMLReader::GenerateXMLTree(XMLNode** node,std::string nodename)
{
	XMLNode* temp;
	XMLNode* move = (*node);
	auto  count = ParentInfo.count(nodename);
	//查找传入节点的所有孩子节点
	auto  iter = ParentInfo.find(nodename);
	
	for (int i = 0; i < count; i++)
	{
		if (i == 0)
		{
			temp->CreateNode(&temp, GetElementContent(XMLCache, iter->second));
			move->firstchild = temp;
			move = temp;
			GenerateXMLTree(&move,iter->second); //第一个节点是传入节点的子节点
		}
		else
		{
			temp->CreateNode(&temp, GetElementContent(XMLCache, iter->second));
			move->nextlibling = temp;
			move = temp;
			GenerateXMLTree(&move, iter->second);//之后所有的节点都是第一个节点的兄弟节点
		}
		iter++;
	}
}

/*读取文件内容到内存*/
XMLNode* XMLReader::XMLFileLoad(const std::string FileName)
{
	//加载文件内容到内存
	std::ifstream read(FileName,std::ios::binary);
	std::stringstream stream;
	stream << read.rdbuf();
	XMLCache = stream.str();
	if (!CheckXML())
	{
		exit(-1);
	}

	//解析xml内容
	ParserNode(XMLCache);

	//初始化根节点,生成XML多叉树
	root->InintNode(&root, GetElementContent(XMLCache, XMLRoot));
	GenerateXMLTree(&root, XMLRoot);
	return root;
}


/*解析成一个个节点并创建多叉树*/
bool XMLReader::ParserNode(const std::string xmlstr)
{
	//XML文件只有一个根元素，将根标签名提取出来做树的根节点
	std::smatch m;
	std::string RootArray;
	std::regex e("<(\\w+).*>|<\\w+[^/>]*/>");
	if (std::regex_search(xmlstr, m, e))  //如果匹配到<> ... </>
	{
		std::string temp = m.str();
		XMLRoot = GetElementName(temp);
	}
	ParseElement(xmlstr);
	return true;
}


/*根据起始标签推导出结束标签 <***> -> </***> */
std::string XMLReader::CreateEndTag(const std::string startTag)
{
	std::string EndTag;
	char EndFlag = '/';

	std::smatch m;
	std::regex e("<\\w+\\b");
	std::regex_search(startTag, m, e); //匹配标签名
	EndTag = m.str();
	EndTag.insert(1, 1, EndFlag);
	EndTag += '>';
	return EndTag;
}

/*获取标签对内的所有内容*/
XMLElement XMLReader::GetElementContent(const std::string XMLCache, const std::string ElementName)
{
	XMLElement temp;
	XMLProperty property;
	std::string content;
	std::string name = ElementName;
	std::string starttag = '<' + ElementName + '>';
	std::string endtag = CreateEndTag(starttag);
	int offset = XMLCache.find(endtag);
	if (offset > 0)    //找到了结束标签，截取从起始到结束的内容
	{
		if (ElementName == XMLRoot)
		{
			content = XMLCache.substr(ElementName.length() + XMLCache.find(ElementName) + 1, XMLCache.find(endtag)-endtag.length());
			content = content.substr(0, content.find(endtag));
		}
		else 
		{
			//截取一个结点的内容做处理
			content = XMLCache.substr(ElementName.length() + XMLCache.find(ElementName) + 1, XMLCache.find(endtag));
			content = content.substr(0, content.find(endtag));
		}
		if (JudgeElemOrText(content)) //content里面含有子元素
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
	else //没找到结束标签，说明是< ... />
	{
		content = XMLCache.substr(XMLCache.find(ElementName) - 1, XMLCache.length());
		content = content.substr(0,content.find("/>"));
		//std::cout << "---------------" << content << "-----------------" << std::endl;
		ParserProperty(content,property);
	}
	temp.emplace(ElementName,property);
	return temp;
}

/*判断各个节点的父子关系，并存储进容器*/
bool XMLReader::ParseElement(std::string Element)
{
	//通过栈结构判断父子节点的关系，每个标签元素入栈，遇到结尾标签出栈，他的父元素就是栈顶元素。
	std::string Search = Element;
	std::stack<std::string> LableStack;
	std::string tempstart;
	std::string tempend;
	std::smatch m;
	std::smatch h;

	//找到起始标签
	std::regex e("<(\\w+).*>|<\\w+[^/>]*/>|</\\w+>");
	LableStack.push(XMLRoot);
	tempstart = XMLRoot;
	std::regex_search(Search, m, e);
	std::string startTag,endTag;
	startTag = m.str();
	endTag = CreateEndTag(startTag);
	Search = Search.substr(startTag.length() + Search.find(startTag), Search.find(endTag) + 1);
	Search = Search.substr(0, Search.find(endTag) - 1);

	while(std::regex_search(Search, m, e))  //如果匹配到<> ... </>
	{
		tempstart = m.str();
		if (GetElementName(m.str()) == LableStack.top()) //如果结束标签和栈顶元素相同，说明标签对结束，栈顶元素出栈
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
		else  //如果结束标签和栈顶元素不同，说明是其子标签，入栈
		{
			if (m.str().find("</") == m.str().npos)
			{
				tempend = "</" + GetElementName(m.str()) + '>'; //恢复结束标签
				if (Element.find(tempend) != Element.npos) //如果找到的有结束标签，则是<>...</>这种
				{
					tempstart = GetElementName(m.str());
					ParentInfo.emplace(LableStack.top(), tempstart);
					LableStack.push(tempstart);
				}
				else   //如果没有标签，则是< .../>这种
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

/*获取标签名*/
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

/*判断是否包含子项目，或者只有一个text特殊属性*/
bool XMLReader::JudgeElemOrText(std::string ElementStr)
{
	std::string s = ElementStr;
	std::smatch m;
	std::regex regexItem("<(\\w+).*>.*</\\1>|<\\w+[^/>]*/>");
	// 匹配：识别成项目
	if (std::regex_search(s, m, regexItem)) {
		return false;
	}
	// 匹配：识别成 text 属性
	else {
		return true;
	}
}

/*解析属性*/
bool XMLReader::ParserProperty(std::string PropertyStr,XMLProperty &Property)
{
	std::string Propertyname;
	std::string Propertyval;
	// 获取：属性名称
	std::string s = PropertyStr;
	std::smatch m;
	std::regex regexAtrributeName("\\b\\w+=");
	if (std::regex_search(s, m, regexAtrributeName)) {
		std::string strTempName = m.str();
		strTempName.erase(strTempName.end() - 1, strTempName.end());
		Propertyname = strTempName;
	}
	// 获取：属性值
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
	std::stack<char> stackBracket;		// 栈：尖括号
	std::stack<char> stackBackSlash;	// 栈：反斜线
	std::stack<std::string> statckTab;  // 栈：标签对
	bool bBracketValid = true;			// 合法：尖括号匹配
	bool bBlackSlashValid = true;		// 合法：反斜线
	bool bIsSymmetry = true;			// 合法：标签对称性
	// 检查：括号匹配
	for (int i = 0; i < XMLCache.size(); ++i) {
		// --记录：当出现 < ，则直接压栈
		if ('<' == XMLCache[i]) stackBracket.push(XMLCache[i]);
		// --记录：当出现 > ，则判断栈是否为空、是否栈顶为 '<'，满足条件则出栈
		else if ('>' == XMLCache[i] && stackBracket.size() != 0 && stackBracket.top() == '<')
			stackBracket.pop();
		// --记录：当出现 / ，则判断栈顶是否为 <、左侧是否有 < 或者右侧是否有 >，
		// 满足条件压栈（计算项目个数），否则报错
		else if ('/' == XMLCache[i] && stackBracket.top() == '<' &&
			((i != 0 && '<' == XMLCache[i - 1]) || (i + 1 != XMLCache.size() && '>' == XMLCache[i + 1])))
			stackBackSlash.push(XMLCache[i]);
		else if ('/' == XMLCache[i]) {
			bBlackSlashValid = false;
			break;
		}
	}
	// 出错：反斜线不匹配
	if (!bBlackSlashValid)
		std::cout << "parse error: black slash write error." << std::endl;
	// 出错：尖括号不匹配
	if (stackBracket.size() != 0) {
		bBracketValid = false;
		std::cout << "parse error: bracket does not matched." << std::endl;
	}
	// 出错：标签对不对称
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
	// 找到双标签对的标签头
	while (std::regex_search(s, m, regexTab)) {
		// --检查:根据匹配的是标签头还是标签尾进行不同的处理
		std::string strTemp = m.str();
		std::smatch tempMatch;
		// --匹配：标签头
		if (std::regex_search(strTemp, tempMatch, regexHeadName)) {
			std::string strTabName = tempMatch.str();
			strTabName.erase(0, 1);
			stackTab.push(strTabName);
		}
		// --匹配：标签尾
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