#pragma once

#include <algorithm>
#include <map>
using namespace std;
//���ز��ҵ���ֵ������ֵ��ȣ������������ֵ,��������ֵ������ʼ���β��
template<typename T_Key,typename T_Val>
inline bool GetMapInterVal(const map<T_Key,T_Val> &_map,const T_Key &findKey,T_Val &preVal,T_Val &lastVal)
{
	if (_map.size() == 0) return false;

	auto iter = _map.find(findKey);
	if (iter != _map.end())
	{
		preVal = iter->second;
		lastVal = iter->second;
		return true;
	}

	iter =  _map.upper_bound(findKey);
	if (iter == _map.begin())
	{
		preVal = iter->second;
		lastVal = iter->second;
	}
	else if (iter == _map.end())
	{
		--iter;
		preVal = iter->second;
		lastVal = iter->second;
	}
	else
	{
		lastVal = iter->second;
		--iter;
		preVal = iter->second;
	}
	return true;
}



//���ز��ҵ���ֵ������ֵ��ȣ������������ֵ,��������ֵ������ʼ���β��
//list/vector..
template<typename T, typename T_Val>
inline bool GetInterVal(const T &_arry,const T_Val &findVal,T_Val &preVal,T_Val &lastVal)
{
	if (_arry.size() == 0) return false;

	auto iter = find(_arry.begin(),_arry.end(),findVal);
	if (iter != _arry.end())
	{
		preVal = *iter;
		lastVal = *iter;
		return true;
	}


	iter = upper_bound(_arry.begin(),_arry.end(),findVal);
	if (iter == _arry.begin())
	{
		preVal = *iter;
		lastVal = *iter;
	}
	else if (iter == _arry.end())
	{
		--iter;
		preVal = *iter;
		lastVal = *iter;
	}
	else
	{
		lastVal = *iter;
		--iter;
		preVal = *iter;
	}

	return true;
}