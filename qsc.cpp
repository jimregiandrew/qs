// $Id$

/*
 * Usage e.g.:
 *  grep system.voltage ~/tmp/cls.xml | wraprootxmlelement.sh | ./qsc -c system.voltage -q 0.5
 */

#include <getopt.h>

#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using std::bind;
using std::cerr;
using std::cout;
using std::endl;
using std::ios;
using std::ostream;
using std::map;
using std::placeholders::_1;
using std::placeholders::_2;
using std::string;
using std::vector;

// http://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
// And see e.g. http://stackoverflow.com/questions/899341/print-coloured-text-to-console-in-c
static const char* RED   ="\033[1;31m";
static const char* NORMAL="\033[0m";



/*******************************************************************************
 *
 *
 *
 *
 *
 * Helpers
 *
 *
 *
 *
 *
 ******************************************************************************/
/*
static smart_ptr<MemoryBuffer> newMemoryBuffer(const char* data, unsigned dataLen)
{
    smart_ptr<MemoryBuffer> buf = new MemoryBuffer;
    buf->sinkReceive(data, dataLen);
    buf->sinkClose();
    return buf;
}

static double mse(const vector<double>& x, const vector<double>& y)
{
    double sumSe = 0;
    int N = x.size();
    for (int n = 0; n < N; ++n)
        sumSe += (x[n]-y[n])*(x[n]-y[n]);
    return sumSe;
}

static double maxError(const vector<double>& x, const vector<double>& y)
{
    double maxErr = 0;
    int N = x.size();
    for (int n = 0; n < N; ++n) {
        double e = fabs(x[n]-y[n]);
        if (e > maxErr)
            maxErr = e;
    }
    return maxErr;
}
*/

/*******************************************************************************
 *
 *
 *
 *
 *
 * ChanLogHandler
 *
 *
 *
 *
 *
 ******************************************************************************/

/*
namespace {

class ChanLogHandler
{
    public:
        ChanLogHandler(const vector<vector<QuantityInfo> >& quantityGroupInfos);
        ~ChanLogHandler();

        void handleChannels(const std::map<std::string, double>&, long long millisec);

        vector<uint8_t> getData();

    private:
        vector<vector<QuantityInfo> > quantityGroupInfos;
        vector<QuantitySequence> groupCoders;
        int32_t startUnixTime;
};

ChanLogHandler::ChanLogHandler(const vector<vector<QuantityInfo> >& quantityGroupInfos)
    : quantityGroupInfos(quantityGroupInfos),
      startUnixTime(0)
{
    for (unsigned int n = 0; n < quantityGroupInfos.size(); ++n) {
        groupCoders.push_back(QuantitySequence(quantityGroupInfos[n]));
    }
}

ChanLogHandler::~ChanLogHandler()
{
}

void ChanLogHandler::handleChannels(const std::map<std::string, double>& channels, long long timeMS)
{
    if (startUnixTime == 0)
        startUnixTime = timeMS / 1000;
    for (unsigned int n = 0; n < quantityGroupInfos.size(); ++n) {
        string channel = quantityGroupInfos[n][0].name;
        if (channels.find(channel) != channels.end()) {
            map<string, double> gpMap;
            const vector<QuantityInfo> gpInfo = quantityGroupInfos[n];
            bool missing = false;
            bool codeUnixTime = false;
            for (unsigned int m = 0; m < gpInfo.size(); ++m) {
                if (gpInfo[m].name != "unixtime") {
                    if (channels.find(gpInfo[m].name) == channels.end()) {
                        cout<<"Missing "<<gpInfo[m].name<<" value: skipping record"<<endl;
                        missing = true;
                        break;
                    }
                    else {
                        gpMap[gpInfo[m].name] = channels.find(gpInfo[m].name)->second;
                    }
                }
                else {
                	codeUnixTime = true;
                }
            }
            if (!missing) {
            	if (codeUnixTime)
            		gpMap["unixtime"] = (double)timeMS/1000.0 - startUnixTime;
                groupCoders[n].handleChannels(gpMap);
            }
        }
    }
}

vector<uint8_t> ChanLogHandler::getData()
{
    return compression::getChannelZipCode(groupCoders, startUnixTime);
}

}
*/


// It is just discipline and faith
/*

 [
{"lat":-36.86, "lng":174.828, "epochMS":1438389103},
{"lat":-36.86, "lng":174.828, "epochMS":1438389104}
]

 */

static string getBetweenQuotes(const string& str)
{
	size_t first = str.find('"');
	if (first == string::npos)
		return str;
	first++;
	size_t last = str.find_last_of('"');
	if (last == string::npos)
		last = str.size();
	return str.substr(first, last-first);
}

using std::ifstream;
using std::istringstream;
static int getQuantities(const vector<string>& quantsSpec, ifstream& is, vector<string>& quants)
{
	int c = is.get();
	while (c != EOF && c != '{')
		c = is.get();
	if (c == EOF)
		return c;

	string quantsStr;
	std::getline(is, quantsStr, '}');
	istringstream iss(quantsStr);
	string quantityStr;
	unsigned n = 0;
	while(std::getline(iss, quantityStr, ','))
	{
	   int idx = quantityStr.find(':');
	   string name = getBetweenQuotes(quantityStr.substr(0, idx));
	   if (name != quantsSpec[n])
		   continue;
	   string value = quantityStr.substr(idx+1);
	   quants[n++] = value;
	}
	if (n != quantsSpec.size()) {
		cerr<<RED<<"Couldn't find quantity="<<quantsSpec[n]<<" in quantityStr="<<quantityStr<<NORMAL<<endl;
		static const int ERROR = -2;
		return ERROR;
	}
	return 0;
}

vector<string> getQuantsSpec(const string& quantsSpecStr)
{
	vector<string> quantsSpec;
	istringstream iss(quantsSpecStr);
	string quantity;
	while(std::getline(iss, quantity, ','))
		quantsSpec.push_back(quantity);
	return quantsSpec;
}

/*******************************************************************************
 *
 *
 *
 *
 *
 * main
 *
 *
 *
 *
 *
 ******************************************************************************/
static void usage(int argc, char* argv[])
{
	(void)argc;
    cerr<<argv[0]<<" [-q quantsSpecStr -t] < stdin"<<endl;
}

int main(int argc, char* argv[])
{
    int opt;
    string quantsSpecStr = "lat,lng,epoch";
    while ((opt = getopt(argc, argv, "q:Q:t:")) != -1) {
        switch (opt) {
            case 'q':
                quantsSpecStr = optarg;
                break;
            case 'Q':
                //qStep = strtod(optarg, NULL);
                break;
            default:
                usage(argc, argv);
                return 1;
        }
    }
    if (optind != argc) {
        cerr<<"No non-optional arguments expected"<<endl;
        usage(argc, argv);
        return -1;
    }
    vector<string> quantsSpec = getQuantsSpec(quantsSpecStr);

    ifstream in("/dev/stdin");
    vector<string> quants(quantsSpec.size());
    using std::cout;
    int ret = 0;
    while ((ret = getQuantities(quantsSpec, in, quants)) != EOF) {
    	if (ret != 0)
    		continue;
    	for (const auto& q : quants)
    		cout<<q<<" ";
    	cout<<endl;
    }
}

/*
    vector<vector<QuantityInfo> > quantityGroupInfos = qs::getQuantityInfoGroups(quantsSpecStr, true);
    cout<<"Coding channels:"<<quantityGroupInfos<<endl;
    ChanLogHandler handler(quantityGroupInfos);
    if (playerType=="channellog") {
        ChannelLogPlayer player(bind(&ChanLogHandler::handleChannels, &handler, _1, _2));
        player.go("/dev/stdin");
    }
    else if (playerType == "logfile") {
        LogFilePlayer player(bind(&ChanLogHandler::handleChannels, &handler, _1, _2));
        player.go("/dev/stdin");
    }
    else {
        cerr<<"playerType="<<playerType<<" not understood (-p option)"<<endl;
        return -1;
    }
    vector<uint8_t> compData = handler.getData();
    cout<<"totalNumBytes="<<compData.size()<<endl;

    std::ofstream out("/dev/stdout", ios::out | ios::binary);
    out.write((const char*)&compData[0], compData.size());

    class RedTracer
{
public:
	RedTracer(ostream& os) : os(os) {}

	template<typename T>
    RedTracer& operator<<(const T& t) {
        os<<RED<<t<<NORMAL;
        return *this;
    }
	RedTracer& operator<<(RedTracer& (*fn)(RedTracer&)) {
        return fn(*this);
    }

private:
    ostream& os;
};


static RedTracer rerr(cerr);

 */
