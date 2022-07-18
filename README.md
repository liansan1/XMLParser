	这是一个用c++编写的简单的XML解析器。
	
	表示xml属性：
	
		struct XMLProperty {
		PropertyKey name;   //属性名，string类型
		PropertyVal text;   //属性值，string类型
	};
	
	一个元素有一个标签，可以有若干个属性：
	
	typedef std::map<ElementName, XMLProperty> XMLElement;
	
	其中ElementName为string类型,这里一个元素有一个标签，一个属性名和一个属性值
	如果一个元素内有多个标签的话，将所有子标签都看成元素的属性值。
	
	一个元素为一个节点：
	
	struct XMLNode {
	public:
		void ShowNode(XMLNode *root);
	private:
		void CreateNode(XMLNode **node, XMLElement element);
		void InintNode(XMLNode **root, XMLElement element);
		
	public:
		XMLElement Element; //节点数据
		XMLNode* firstchild; 
		XMLNode* nextlibling;  
	};
	
	通过孩子兄弟表示法将xml中每个元素表示成一颗多叉树，每个节点都包含一个
	元素值，一个指向孩子的指针，一个指向兄弟的指针。
	程序将XML文件表示成树后就可以调用结构体里面的函数进行增删改查。
	
	程序的运行流程主要：
		1.程序先读取内容到内存
		
			XMLReader::LoadFile()是对外的接口，它返回一个XMLNode类型的指针，
		
		2.然后检查xml文件是否合法
		
			XMLReader::CheckXML()
		
		3.解析缓存使用multimap容器记录父子关系
		
			std::multimap<std::string Parent, std::string Child> ParentInfo; 
			Parent：父
			Child:子
			
		4.利用multimap容器生成多叉树
		
			1）有相同父标签的所有子标签互为兄弟标签
			2）key中没有的标签说明是叶子标签
			利用这两条性质完成对多叉树的创建
			
		5.显示数据
			这里只做了对数据的遍历，以后可以完善成对树的增删改从而实现
			对xml文档的增删改，还有待完善。
	
	该程序所使用到的知识点：
		1.左右孩子表示法创建多叉树
		2.栈
		3.正则表达式
		4.map、multimap
	
	
	这份程序还有许多的不足：
		1.标签名不能重复
		2.不支持多个属性，比如<name a="a" b="b"/>这样
		3.对XML文档合法性检查还不够严谨
		4.含有子元素的父元素解析的不够彻底，只是将父元素标签内的所有子元素
			都存进了树中。