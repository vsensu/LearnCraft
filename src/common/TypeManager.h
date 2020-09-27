//
// Created by vsensu on 2020/9/20.
//

#ifndef LEARNCRAFT_SRC_COMMON_TYPEMANAGER_H
#define LEARNCRAFT_SRC_COMMON_TYPEMANAGER_H

#include <unordered_map>
#include <functional>

template<typename IDType, typename DataType>
class TypeManager
{
public:
    virtual ~TypeManager() {}

	[[nodiscard]] virtual bool RegisterType(const IDType &t, const DataType &data) { typeid_typedata_[t] = data; return true; }
	[[nodiscard]] inline const DataType& GetTypeData(const IDType &t) { return typeid_typedata_[t]; }
	void ForEach(std::function<void(const IDType&, const DataType&)> func)
	{
		for(const auto &pair : typeid_typedata_)
		{
			func(pair.first, pair.second);
		}
	}

protected:
	std::unordered_map<IDType, DataType> typeid_typedata_;
};

#endif //LEARNCRAFT_SRC_COMMON_TYPEMANAGER_H
