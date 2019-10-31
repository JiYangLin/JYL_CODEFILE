#pragma once

#include<boost\archive\text_iarchive.hpp>
#include <boost\archive\text_oarchive.hpp>
#include <boost\serialization\vector.hpp>  
#include <boost\serialization\map.hpp>  
#include <boost\serialization\list.hpp> 
#include <boost\serialization\set.hpp>  
#include <boost/filesystem.hpp>
#include <typeinfo>
#include<sstream>
using namespace std;

template<typename Element>
class ArchiveOpt
{
public:
	ArchiveOpt(unsigned  int _flushCount = 10000, string filePath = "D:\\") : m_flushCount(_flushCount)
	{
		cout << sizeof(Element);
		m_filePath = filePath + typeid(Element).name() + "\\";
		Relase();
		boost::filesystem::create_directory(boost::filesystem::path(m_filePath));
	}
	void Store(Element *dat)
	{
		AppendDat(dat);
		if (flushIfNeed())
		{
			flush();
		}
	}
	void Relase()
	{
		if (boost::filesystem::exists(m_filePath))
		{
			boost::filesystem::remove_all(boost::filesystem::path(m_filePath));
		}
	}
	virtual void ForEach(const function<void(Element&)>& callback) = 0;
	virtual void flush() = 0;
protected:
	virtual void AppendDat(Element *dat) = 0;
	virtual bool flushIfNeed() = 0;

protected:
	void GetFileDat(string fileFullPath, const function<void(Element&)>& Callback)
	{
		fstream iof(fileFullPath);
		if (!iof) return;

		string strline;

		while (getline(iof, strline))
		{
			stringstream ss(strline);
			boost::archive::text_iarchive ia(ss);

			size_t DatCount = 0;
			ia >> DatCount;//获取每行数据个数
			for (size_t i = 0; i < DatCount; i++)
			{
				Element  *dat = new Element();
				ia >> *dat;
				AppendDat(dat);
			}

			ProcessPerLine(Callback);
		}
	}
	virtual void ProcessPerLine(const function<void(Element&)>& Callback) {}

	void SaveLineDat(string fileFullPath,list<Element*> &DatList)
	{
		fstream iof(fileFullPath, fstream::out | fstream::app);
		if (!iof) return;
		boost::archive::text_oarchive oa(iof);

		oa << DatList.size();//每行数据第一个存储数据一行包含的数据条数
		for_each(DatList.begin(), DatList.end(), [&](Element* ele) { oa << (*ele);});
	}
protected:
	string m_filePath;
	unsigned int m_flushCount;
};


template<typename Element>
class ArchiveMap :public ArchiveOpt<Element>
{
public:
	ArchiveMap(unsigned  int _flushCount = 10000, string filePath = "D:\\") :ArchiveOpt(_flushCount, filePath) {}

	void ForEach(const function<void(Element&)>& callback)
	{
		flush();

		for (auto it = m_FilePathInfo.begin();it != m_FilePathInfo.end(); ++it)
		{
			GetFileDat(*it, [&](Element& el) {});

			for (auto it = m_Dat.begin(); it != m_Dat.end(); ++it)
			{
				callback(*it->second);
			}
			ClearDat();
		}
	}
	void flush()
	{
		//根据fileID分类数据
		map<typename Element::ID_TYPE, list<Element*>> FileID_Dat;
		GetFileID_Dat(FileID_Dat);

		//按照文件ID进行存储
		for (auto it = FileID_Dat.begin(); it != FileID_Dat.end(); ++it)
		{
			stringstream ss;
			ss << "Archive_" << it->first;
			string fileFullPath = m_filePath + ss.str();
			(void)m_FilePathInfo.insert(fileFullPath);

			SaveLineDat(fileFullPath,it->second);
		}

		ClearDat();
	}
protected:
	bool flushIfNeed()
	{
		if (m_Dat.size() > flushCount)
		{
			return true;
		}
		return false;
	}
protected:
	void AppendDat(Element *dat)
	{
		typename Element::ID_TYPE  id = dat->ID();
		if (m_Dat.end() == m_Dat.find(id))
		{
			m_Dat[id] = dat;
		}
		else
		{
			m_Dat[id]->Merge(*dat);
			delete dat;
		}
	}
	void ClearDat()
	{
		for (auto it = m_Dat.begin(); it != m_Dat.end(); ++it)
		{
			delete it->second;
		}
		m_Dat.swap(map<typename Element::ID_TYPE, Element*>());
	}
	void GetFileID_Dat(map<typename Element::ID_TYPE, list<Element*>> &FileID_Dat)
	{//根据fileID分类数据
		for (auto it = m_Dat.begin(); it != m_Dat.end(); ++it)
		{
			typename Element::ID_TYPE fileID = it->second->FileID();
			FileID_Dat[fileID].push_back(it->second);
		}
	}
protected:
    map<typename Element::ID_TYPE, Element*> m_Dat;
	unsigned  int flushCount;
	
	string m_filePath;
	
	set<string> m_FilePathInfo;
};


template<typename Element>
class ArchiveList :public ArchiveOpt<Element>
{
public:
	ArchiveList(unsigned  int _flushCount = 10000, string filePath = "D:\\") :ArchiveOpt(_flushCount, filePath) 
	{
		m_FileFullPath = m_filePath + "ListFile";
	}
	virtual void ForEach(const function<void(Element&)>& callback)
	{
		flush();
		GetFileDat(m_FileFullPath,callback);
	}
	virtual void flush() 
	{
		SaveLineDat(m_FileFullPath, m_Dat);
		clearDat();
	};
protected:
	virtual void ProcessPerLine(const function<void(Element&)>& Callback)
	{
		for (auto it = m_Dat.begin(); it != m_Dat.end(); ++it)
		{
			Callback(**it);
		}
		clearDat();
	}
	virtual void AppendDat(Element *dat) 
	{
		m_Dat.push_back(dat);
	};
	virtual bool flushIfNeed()
	{
		if (m_Dat.size() > m_flushCount)
		{
			return true;
		}
		return false;
	}
protected:
	void clearDat()
	{
		for_each(m_Dat.begin(), m_Dat.end(), [&](Element* ele) { delete ele;});
		m_Dat.swap(list<Element*>());
	}
protected:
	string m_FileFullPath;
	list<Element*> m_Dat;
};



////////////////// ArchiveMap  Test////////////
class TestMapStu
{
public:
	typedef int ID_TYPE;
	int id;
	set<int> dat;

	vector<int>  vec;

	ID_TYPE  ID() { return id; }
	ID_TYPE  FileID() { return id % 10; }
	void Merge(TestMapStu& other)
	{
		for each (int a in other.dat)
		{
			dat.insert(a);
		}
	}
public:
	TestMapStu() {};
	TestMapStu(int d, int a) :id(d)
	{
		dat.insert(a);
		vec.resize(1000);
	}

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & id;
		ar & dat;
		ar & vec;
	}
};
inline void testMapFun()
{
	ArchiveMap<TestMapStu> Te;


	Te.Store(new TestMapStu(1, 1));
	Te.Store(new TestMapStu(1, 2));
	Te.Store(new TestMapStu(33, 4));
	Te.Store(new TestMapStu(2, 5));
	Te.Store(new TestMapStu(3, 77));
	Te.Store(new TestMapStu(4, 1));
	Te.Store(new TestMapStu(1, 9));
	Te.Store(new TestMapStu(6, 1));
	Te.Store(new TestMapStu(6, 2));
	Te.Store(new TestMapStu(1, 9));

	Te.ForEach([&](TestMapStu& elment)
	{
		cout << "id:" << elment.id << " dat: ";
		for each (int a in elment.dat)
		{
			cout << a << "  ";
		}

		cout << endl;
	});

	Te.Relase();
}

////////////////// ArchiveList  Test////////////
class Te
{
public:
	Te() {}
	Te(int _id) :id(_id)
	{
		vec.resize(100, id);
	}
	int id;
	vector<int> vec;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & id;
		ar & vec;
	}
};
inline void TestList()
{
	ArchiveList<Te>  l(100);
	for (size_t i = 5; i < 100; i++)
	{
		l.Store(new Te(i));
	}
	l.ForEach([&](Te& t)
	{
		cout << t.id << endl;
	});
	cout << endl;
	l.Relase();
}
