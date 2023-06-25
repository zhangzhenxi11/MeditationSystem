#pragma once

#include <QMap>
#include <QByteArray>
#include <QPixmap>
#include <QFile>
#include <QDateTime>
#include <QVariant>
#include <QObject>
#include <QMetaEnum>
#include <QStringList>
#include <QDataStream>

#pragma region 通用配置

#define THREADSLEEP 1
#define	TIMEFORMAT "yyyy-MM-dd hh:mm:ss"
#define	TIMEFORMAT2 "yyyy-MM-dd hh:mm:ss.zzz"

template<typename T>
static QStringList typeNameList()
{
	QStringList list;
	QMetaEnum metaEnum = QMetaEnum::fromType<T>();

	for (int i = 0; i < metaEnum.keyCount(); i++) {
		list += metaEnum.valueToKey(metaEnum.value(i));
	}
	return list;
};
template<typename T>
static T nameToType(const QString& name, bool *isok = nullptr)
{
	QMetaEnum metaEnum = QMetaEnum::fromType<T>();
	T type = (T)metaEnum.keyToValue(name.toLocal8Bit(), isok);
	return type;
};
template<typename T>
static QString typeToName(const T& type)
{
	QMetaEnum metaEnum = QMetaEnum::fromType<T>();
	QString name = metaEnum.valueToKey(type);
	return name;
};

//参数
class cValue : public QObject
{
	Q_OBJECT
public:
	//值种类
	enum nValueType
	{
		Invalid = QVariant::Type::Invalid,
		Bool = QVariant::Type::Bool,
		Int = QVariant::Type::Int,
		Double = QVariant::Type::Double,
		Float = QMetaType::Float,
		String = QVariant::Type::String,
		StringList = QVariant::Type::StringList,
		ByteArray = QVariant::Type::ByteArray,
		DateTime = QVariant::Type::DateTime,
		Rect = QVariant::Type::Rect,
		RectF = QVariant::Type::RectF,
		Size = QVariant::Type::Size,
		SizeF = QVariant::Type::SizeF,
		Line = QVariant::Type::Line,
		LineF = QVariant::Type::LineF,
		Point = QVariant::Type::Point,
		PointF = QVariant::Type::PointF,
		Polygon = QVariant::Type::Polygon,
		PolygonF = QVariant::Type::PolygonF,
	};
	Q_ENUM(nValueType)

	enum nParaType
	{
		Default,		//默认参数
		Custom,			//自定义参数
		Temp,			//临时参数
	};
	Q_ENUM(nParaType)

	cValue(int type = Default) :QObject(), iParaType(type){};
	cValue(const cValue &other){
		strName = other.strName;
		strDescribe = other.strDescribe;
		varValue = other.varValue;
		iParaType = other.iParaType;
	};
	~cValue(){};

	//供map的key值使用
//	bool operator< (const cValue& other) const   //注: const 不能省
//	{
//		if (strName < other.strName)
//		{
//			return true;
//		}
//		else if (strName == other.strName)
//		{
//			if (varValue < other.varValue)
//			{
//				return true;
//			}
//		}
//		return false;
//	}
	bool operator== (const cValue& other) const   //注: const 不能省
	{
		if (strName == other.strName
			&& varValue == other.varValue)
		{
			return true;
		}
		return false;
	}
	cValue &operator=(const cValue &other)
	{
		strName = other.strName;
		strDescribe = other.strDescribe;
		varValue = other.varValue;
		iParaType = other.iParaType;
		return *this;
	};
	bool isValid() const
	{
		return (!strName.isEmpty()
			&& varValue.isValid());
	}

	bool valueToShow(QString& show) const
	{
		bool ret = false;
		switch (varValue.type())
		{
			case cValue::Invalid:
			{
				show = "Invalid";
				ret = false;
				break;
			}			
			case cValue::Bool:
			{
				show = varValue.toBool() ? QString::fromLocal8Bit("是") : QString::fromLocal8Bit("否");
				ret = true;
				break;
			}			
			case cValue::Int:
			{
				show = QString::number(varValue.toInt(&ret));
				break;
			}	
			case cValue::Double:
			{
				show = QString::number(varValue.toDouble(&ret), 'f', 3);
				break;
			}		
			case cValue::Float:
			{
				show = QString::number(varValue.toFloat(&ret), 'f', 3);
				break;
			}
			case cValue::String:
			{
				show = varValue.toString();
				ret = true;
				break;
			}	
			case cValue::StringList:
			{
				show = varValue.toStringList().join(",");
				ret = true;
				break;
			}		
			case cValue::ByteArray:
			{
				show = QString::fromLocal8Bit(varValue.toByteArray());
				ret = true;
				break;
			}			
			case cValue::DateTime:
			{
				show = varValue.toDateTime().toString("yyyy/MM/dd hh:mm:ss");
				ret = true;
				break;
			}		
			case cValue::Rect:
			{
				QRect rect = varValue.toRect();
				show = QString("(%1,%2,%3,%4)").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
				ret = true;
				break;
			}
			case cValue::RectF:
			{
				QRectF rect = varValue.toRectF();
				show = QString("(%1,%2,%3,%4)").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
				ret = true;
				break;
			}			
			case cValue::Size:
			{
				QSize size = varValue.toSize();
				show = QString("(%1,%2)").arg(size.width()).arg(size.height());
				ret = true;
				break;
			}		
			case cValue::SizeF:
			{
				QSizeF size = varValue.toSizeF();
				show = QString("(%1,%2)").arg(size.width()).arg(size.height());
				ret = true;
				break;
			}		
			case cValue::Line:
			{
				QLine line = varValue.toLine();
				show = QString("(%1,%2) (%3,%4)").arg(line.x1()).arg(line.y1()).arg(line.x2()).arg(line.y2());
				ret = true;
				break;
			}		
			case cValue::LineF:
			{
				QLineF line = varValue.toLineF();
				show = QString("(%1,%2) (%3,%4)").arg(line.x1()).arg(line.y1()).arg(line.x2()).arg(line.y2());
				ret = true;
				break;
			}		
			case cValue::Point:
			{
				QPoint point = varValue.toPoint();
				show = QString("(%1,%2)").arg(point.x()).arg(point.y());
				ret = true;
				break;
			}		
			case cValue::PointF:
			{
				QPointF point = varValue.toPointF();
				show = QString("(%1,%2)").arg(point.x()).arg(point.y());
				ret = true;
				break;

			}	
			//case cValue::Polygon:
			//{
			//	QPolygon polygon = varValue.value<QPolygon>();
			//	for (auto point: polygon)
			//	{
			//		show.append(QString("(%1,%2)").arg(point.x()).arg(point.y()));
			//		show.append("\r\n");
			//	}
			//	ret = true;
			//	break;

			//}
			//case cValue::PolygonF:
			//{
			//	QPolygonF polygon = varValue.value<QPolygonF>();
			//	for (auto point : polygon)
			//	{
			//		show.append(QString("(%1,%2)").arg(point.x()).arg(point.y()));
			//		show.append("\r\n");
			//	}
			//	ret = true;
			//	break;
			//}
			default:
			{
				show = "Invalid";
				ret = false;
				break;
			}
		}
		return ret;
	};

	bool showToValue(const QString& show)
	{
		bool ret = false;
		switch (varValue.type())
		{
			case cValue::Invalid:
			{
				ret = false;
				break;
			}			
			case cValue::Bool:
			{
				if (show == QString::fromLocal8Bit("是"))
				{
					varValue = true;
					ret = true;
				}
				else if (show == QString::fromLocal8Bit("否"))
				{
					varValue = false;
					ret = true;
				}			
				break;
			}			
			case cValue::Int:
			{
				varValue = show.toInt(&ret);
				break;
			}	
			case cValue::Double:
			{
				varValue = show.toDouble(&ret);
				break;
			}		
			case cValue::Float:
			{
				varValue = (double)show.toFloat(&ret);
				break;
			}
			case cValue::String:
			{
				varValue = show;
				ret = true;
				break;
			}	
			case cValue::StringList:
			{
				varValue = show.split(",");
				ret = true;
				break;
			}		
			case cValue::ByteArray:
			{
				varValue = show.toLocal8Bit();
				ret = true;
				break;
			}			
			case cValue::DateTime:
			{
				varValue = QDateTime::fromString(show, "yyyy/MM/dd hh:mm:ss");
				ret = true;
				break;
			}		
			case cValue::Rect:
			{
				QString tmp = show.mid(1);
				tmp.chop(1);
				QStringList tmplst = tmp.split(",");
				if (tmplst.count() == 4)
				{
					varValue = QRect(tmplst.at(0).toInt(), tmplst.at(1).toInt(), tmplst.at(2).toInt(), tmplst.at(3).toInt());
					ret = true;
				}		
				break;
			}
			case cValue::RectF:
			{
				QString tmp = show.mid(1);
				tmp.chop(1);
				QStringList tmplst = tmp.split(",");
				if (tmplst.count() == 4)
				{
					varValue = QRectF(tmplst.at(0).toFloat(), tmplst.at(1).toFloat(), tmplst.at(2).toFloat(), tmplst.at(3).toFloat());
					ret = true;
				}
				break;
			}			
			case cValue::Size:
			{
				QString tmp = show.mid(1);
				tmp.chop(1);
				QStringList tmplst = tmp.split(",");
				if (tmplst.count() == 2)
				{
					varValue = QSize(tmplst.at(0).toInt(), tmplst.at(1).toInt());
					ret = true;
				}
				break;
			}		
			case cValue::SizeF:
			{
				QString tmp = show.mid(1);
				tmp.chop(1);
				QStringList tmplst = tmp.split(",");
				if (tmplst.count() == 2)
				{
					varValue = QSize(tmplst.at(0).toFloat(), tmplst.at(1).toFloat());
					ret = true;
				}
				break;
			}		
			case cValue::Line:
			{
				QStringList tmplst = show.split(" ");
				if (tmplst.count() != 2)
				{
					ret = false;
					break;
				}
				QString tmp = tmplst.at(0).mid(1);
				tmp.chop(1);
				QStringList tmplst2 = tmp.split(",");
				if (tmplst2.count() != 2)
				{
					ret = false;
					break;
				}
				int x1 = tmplst2.at(0).toInt(&ret);
				int y1 = tmplst2.at(1).toInt(&ret);

				QString tmp2 = tmplst.at(1).mid(1);
				tmp2.chop(1);
				QStringList tmplst3 = tmp2.split(",");
				if (tmplst3.count() != 2)
				{
					ret = false;
					break;
				}
				int x2 = tmplst3.at(0).toInt(&ret);
				int y2 = tmplst3.at(1).toInt(&ret);

				varValue = QLine(x1, y1, x2, y2);
				break;
			}		
			case cValue::LineF:
			{
				QStringList tmplst = show.split(" ");
				if (tmplst.count() != 2)
				{
					ret = false;
					break;
				}
				QString tmp = tmplst.at(0).mid(1);
				tmp.chop(1);
				QStringList tmplst2 = tmp.split(",");
				if (tmplst2.count() != 2)
				{
					ret = false;
					break;
				}
				float x1 = tmplst2.at(0).toFloat(&ret);
				float y1 = tmplst2.at(1).toFloat(&ret);

				QString tmp2 = tmplst.at(1).mid(1);
				tmp2.chop(1);
				QStringList tmplst3 = tmp2.split(",");
				if (tmplst3.count() != 2)
				{
					ret = false;
					break;
				}
				float x2 = tmplst3.at(0).toFloat(&ret);
				float y2 = tmplst3.at(1).toFloat(&ret);

				varValue = QLineF(x1, y1, x2, y2);
				ret = true;
				break;
			}		
			case cValue::Point:
			{
				QString tmp = show.mid(1);
				tmp.chop(1);
				QStringList tmplst = tmp.split(",");
				if (tmplst.count() == 2)
				{
					varValue = QPoint(tmplst.at(0).toInt(), tmplst.at(1).toInt());
					ret = true;
				}
				break;
			}		
			case cValue::PointF:
			{
				QString tmp = show.mid(1);
				tmp.chop(1);
				QStringList tmplst = tmp.split(",");
				if (tmplst.count() == 2)
				{
					varValue = QPointF(tmplst.at(0).toFloat(), tmplst.at(1).toFloat());
					ret = true;
				}
				break;

			}
			default:
			{
				ret = false;
				break;
			}
		}
		return ret;
	};

public:
	QString					strName;				//参数名称
	QString					strDescribe;				//参数描述
	QVariant				varValue;					//参数值
	int						iParaType;					//参数种类
};
Q_DECLARE_METATYPE(cValue)
inline QDataStream& operator<<(QDataStream& out, const cValue& info)
{
	out << info.strName << info.strDescribe << info.varValue << info.iParaType;
	return out;
};
inline QDataStream& operator>>(QDataStream& in, cValue& info)
{
	in >> info.strName >> info.strDescribe >> info.varValue >> info.iParaType;
	return in;
};

//算法参数描述
struct sParamDescribe
{
	QString			strName;
	QString			strDescribe;
	int				iIndex;

	//供map的key值使用
	bool operator< (const sParamDescribe& other) const   //注: const 不能省
	{
		if (iIndex < other.iIndex)
		{
			return true;
		}
		else if (iIndex == other.iIndex)
		{
			if (strName < other.strName)
			{
				return true;
			}
			else if (strName == other.strName)
			{
				if (strDescribe < other.strDescribe)
				{
					return true;
				}
			}
		}
		return false;
	}

	sParamDescribe &operator=(const sParamDescribe &other)
	{
		strName = other.strName;
		strDescribe = other.strDescribe;
		iIndex = other.iIndex;
		return *this;
	};
};
Q_DECLARE_METATYPE(sParamDescribe)
inline QDataStream& operator<<(QDataStream& out, const sParamDescribe& info)
{
	out << info.strName << info.strDescribe << info.iIndex;
	return out;
};
inline QDataStream& operator>>(QDataStream& in, sParamDescribe& info)
{
	in >> info.strName >> info.strDescribe >> info.iIndex;
	return in;
};

typedef  QMap<sParamDescribe, QMap<sParamDescribe, QVariant>> MapParams;

inline QDataStream& operator<<(QDataStream& out, const QMap<sParamDescribe, QVariant>& info)
{
	int count = info.count();
	out << count;
	for (auto itmap = info.constBegin(); itmap != info.constEnd(); ++itmap)
	{
		out << itmap.key() << itmap.value();
	}
	return out;
};
inline QDataStream& operator>>(QDataStream& in, QMap<sParamDescribe, QVariant>& info)
{
	int count;
	in >> count;
	for (int i = 0; i < count; i++)
	{
		sParamDescribe key;
		QVariant value;
		in >> key >> value;
		info.insert(key, value);
	}
	return in;
};
Q_DECLARE_METATYPE(MapParams)

inline QDataStream& operator<<(QDataStream& out, const MapParams& info)
{
	int count = info.count();
	out << count;
	for (auto itmap = info.constBegin(); itmap != info.constEnd(); ++itmap)
	{
		out << itmap.key() << itmap.value();
	}
	return out;
};
inline QDataStream& operator>>(QDataStream& in, MapParams& info)
{
	int count;
	in >> count;
	for (int i = 0; i < count; i++)
	{
		sParamDescribe key;
		QMap<sParamDescribe, QVariant> value;
		in >> key >> value;
		info.insert(key, value);
	}
	return in;
};

static void updateParams(const cValue& newparam, QList<cValue>& params)
{
	if (params.isEmpty())
		return;
	bool isfound = false;
	for (auto& val : params)
	{
		if (val.strName == newparam.strName)
		{
			val = newparam;
			isfound = true;
		}
	}
	if (!isfound)
	{
		params.append(newparam);
	}
}

#pragma endregion
#pragma region 界面相关


#pragma endregion
#pragma region 命令相关

#define SPLIT ";"
#define SPLIT2 "~"
#define SPLIT3 "@"

//模块初始化执行的命令
#define DEVCINIT "devcinit"
//模块关闭前执行的命令
#define DEVCUNINIT "devcuninit"
//启动模块命令
#define DEVCSTART "devcstart"
//继续模块命令
#define DEVCCONSTART "devcconstart"
//停止模块命令
#define DEVCSTOP "devcstop"
//测试模块命令
#define DEVCTEST "devctest"



#define MODELPATH "/Model/"
#define MODELFILE "*.model"

#define TREEPATH "/Model/"
#define TREEFILE "*.tree"

#define FILTERPATH "/Model/"
#define FILTERFILE "*.filter"

#define ALGOPATH "/Model/"
#define ALGOFILE "*.algo"

#define DEVCPATH "/Model/"
#define DEVCFILE "*.devc"

#define CAMPATH "/Model/"
#define CAMFILE "*.eng"

#define ENGPATH "/Model/"
#define ENGFILE "*.engex"

#define CONFIGPATH	"/Config/"
#define CONFIG		"config.ini"
#define UI_CONFIG	"ui.ini"

//#define PLUGINPATH "/Plugin"

#define PLUGINPATH ""

#define LICENSEPATH "/license"

#define PARAMFILE "*.param"

#define DBINIT "/DBInit"
#define DBPATH "/DB"
#define DBNAME "/RS-ResultData.db"
#define DBCONFIG "/RS-Config.db"

#define CONFIGDBNAME "ConfigData"
#define RESULTDB "ResultData"
#define STATIONTABLE "Station"
#define PRODUCTABLE "Product"
#define DEFECTTABLE "Defect"
#define FILETABLE "File"

#define CONFIGTABLE "CommonConfig"
#define PRODUCTRECORDTABLE "ProductRecord"


enum nRights
{
	Rights_User = 0,									//用户
	Rights_Debugger,									//调试员
	Rights_Developer,									//开发者
};
