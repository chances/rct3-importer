///////////////////////////////////////////////////////////////////////////////
//
// autover
// Command line versioning tool
// Copyright (C) 2008 Tobias Minch
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, <http://www.gnu.org/licenses/>.
//
// Written by
//   Tobias Minich - belgabor@gmx.de
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/exception.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
namespace fs = boost::filesystem;
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH
#include <boost/format.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;
#include <boost/regex.hpp>
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_chset.hpp>
#include <boost/spirit/include/classic_confix.hpp>
#include <boost/spirit/include/classic_push_back_actor.hpp>
#include <boost/variant.hpp>

#include <exception>
#include <iostream>
//#include <fstream>
#include <iterator>
#include <list>
#include <map>
#include <vector>

using namespace std;
using namespace BOOST_SPIRIT_CLASSIC_NS;
using boost::lexical_cast;

template<class C, class T>
inline bool has ( C& container, const T& value ) {
    return container.find(value) != container.end();
}

template<class C, class T>
inline bool has ( const C& container, const T& value ) {
    return container.find(value) != container.end();
}

#define APPNAME "autover"

#define TOKEN_VARIABLE "var"
#define TOKEN_DATE "date"
#define TOKEN_TIME "time"
#define TOKEN_COMPLEX "multi"
#define TOKEN_TIMEVAR "timevar"
#define TOKEN_SET "set"

typedef boost::variant<string, long> av_value;
typedef map<string, av_value> av_map;
typedef pair<string, av_value> av_map_pair;

typedef boost::error_info<struct tag_message, string> ex_message;
typedef boost::error_info<struct tag_file, string> ex_file;
typedef boost::error_info<struct tag_module, string> ex_module;
typedef boost::error_info<struct tag_line, string> ex_line;
typedef boost::error_info<struct tag_lineno, long> ex_lineno;
typedef boost::error_info<struct tag_usage, bool> ex_usage;

class eautover: public boost::exception, public exception { };

struct av_state {
	//string comment;
	//string string_delim;
	//string line_delim;
	unsigned int verbosity;
	boost::posix_time::ptime now;
	av_map settings;
	av_map variables;
	
	vector<string> counters;
	bool do_date;
	bool do_time;
	bool do_external;
	
	bool strict_mode;
	bool do_update;
	
	av_state():
		//comment("//"),
		//string_delim("\"'"),
		//line_delim(";"),
		verbosity(1),
		now(boost::posix_time::second_clock::local_time()),
		do_date(false),
		do_time(false),
		do_external(false),
		strict_mode(false),
		do_update(false)
	{
		// Setup defaults
		settings["comment"] = "//";
		settings["string_delim"] = "\"'";
		settings["line_delim"] = ";";

		settings["ext:command"] = string("svn info --xml \"%s\"");
		settings["ext:regex:svnrev"] = string(".*revision=\"(.*)\".*");
		settings["ext:regexno:svnrev"] = 1;
		settings["ext:regex:svndate"] = string(".*<date>(.*)</date>.*");
		settings["ext:regexno:svndate"] = 1;
		settings["ext:parameter"] = string(".");
		settings["ext:parameter:path"] = 1;
	}
	inline string comment() { return lexical_cast<string>(settings["comment"]); }
	inline string string_delim() { return lexical_cast<string>(settings["string_delim"]); }
	inline string line_delim() { return lexical_cast<string>(settings["line_delim"]); }
};

struct gr_comment: public grammar<gr_comment>
{
	struct pusher {
		pusher(vector<string>& v_) : v(v_) {}
		void operator()(const string& s) const { v.push_back(s); }
		vector<string>& v;
	};
	
	template <typename ScannerT>
	struct definition
	{
		definition(gr_comment const& self)
		{
			token_r = (+(anychar_p - chset<>("\":'")))[push_back_a(self.v)];
			//any_r = (+anychar_p)[push_back_a(self.v)];
			format_r = ('"' >> (+~ch_p('"'))[push_back_a(self.v)] >> '"') |
					   ('\'' >> (+~ch_p('\''))[push_back_a(self.v)] >> '\'');
			
			variable_r = str_p(TOKEN_VARIABLE)[push_back_a(self.v)] >>
						 ':' >>
						 token_r >>
						 !( ':' >> format_r );
			timevar_r = str_p(TOKEN_TIMEVAR)[push_back_a(self.v)] >>
						':' >>
						token_r >>
						':' >> format_r;
			datetime_r = (str_p(TOKEN_DATE) | str_p(TOKEN_TIME))[push_back_a(self.v)] >>
						':' >>
						format_r;
			complex_r = str_p(TOKEN_COMPLEX)[push_back_a(self.v)] >>
						':' >>
						+(token_r >> ':') >>
						format_r;
			token_setting_ch_r = alnum_p | ':';
			token_setting_r = (token_setting_ch_r >> (token_setting_r | ':'))[push_back_a(self.v)];
			setting_r = str_p(TOKEN_SET)[push_back_a(self.v)] >>
						':' >>
						token_setting_r >>
						format_r;
						  
			first = variable_r | complex_r | timevar_r | datetime_r | setting_r;
		}
		
        rule<ScannerT> first;
		
        rule<ScannerT> token_r;
        rule<ScannerT> token_setting_r;
        rule<ScannerT> token_setting_ch_r;
        //rule<ScannerT> any_r;
        rule<ScannerT> format_r;
		
        rule<ScannerT> variable_r;
        rule<ScannerT> datetime_r;
		rule<ScannerT> complex_r;
		rule<ScannerT> timevar_r;
		rule<ScannerT> setting_r;
		
        rule<ScannerT> const& start() const { return first; }
    };

    gr_comment(vector<string>& v_): v(v_) {}
	vector<string>& v;
};

void output(av_state& state, unsigned int verbosity, const string& message, const string& message2 = "", bool add_endl = true) {
	if (state.verbosity >= verbosity) {
		cerr << message << message2;
		if (add_endl)
			cerr << endl;
	}
}

void warn(av_state& state, const string& message) {
	if (state.strict_mode)
		throw eautover() << ex_message(message);
	else
		output(state, 1, "Warning: ", message);
}

bool parseComment(string& line, vector<string>& tokens) {
	gr_comment gr(tokens);
	return parse(line.c_str(), gr).full;
}

bool parseTime(const string& i, boost::posix_time::ptime& o) {
	unsigned int year, month, day, hour, min, sec, microsec;
	uint_parser<unsigned, 10, 6, 6> six_p;
	uint_parser<unsigned, 10, 4, 4> four_p;
	uint_parser<unsigned, 10, 2, 2> two_p;
	rule<> date_r = four_p[assign_a(year)] >> '-' >> two_p[assign_a(month)] >> '-' >> two_p[assign_a(day)];
	rule<> time_r = two_p[assign_a(hour)] >> ':' >> two_p[assign_a(min)] >> ':' >> two_p[assign_a(sec)] >> '.' >> six_p[assign_a(microsec)];
	if (parse(i.c_str(), date_r >> 'T' >> time_r >> 'Z', nothing_p).full) {
		o = boost::posix_time::ptime(boost::gregorian::date(year, month, day), boost::posix_time::time_duration(hour, min, sec) + boost::posix_time::microseconds(microsec));
		return true;
	}
	try {
		o = boost::posix_time::time_from_string(i);
		return true;
	} catch(...) {}
	try {
		stringstream ss(i);
		ss >> o;
		return !o.is_not_a_date_time();
	} catch(...) {}
	return false;
}

class lineParser {
public:
	enum TYPE {
		NONE,
		DATE,
		TIME,
		VARIABLE,
		COMPLEX,
		TIMEVAR
	};

	lineParser(const char* line, unsigned int lineno, av_state& state): m_state(state), m_line(line), m_lineno(lineno), m_process(NONE) {
		parseLine();
	}
	lineParser(const string& line, unsigned int lineno, av_state& state): m_state(state), m_line(line), m_lineno(lineno), m_process(NONE) {
		parseLine();
	}
	
	void writeLine(ostream& out) const {
		try {
			if (m_process == VARIABLE) {
				outWrap(out, boost::str(boost::format(m_format) % m_state.variables[m_variables[0]]));
				return;
			} else if (m_state.do_date && (m_process == DATE)) {
				stringstream ss;
				boost::gregorian::date_facet f(m_format.c_str());
				ss.imbue(locale(out.getloc(), &f));
				ss << m_state.now.date();
				outWrap(out, ss.str());
				return;
			} else if (m_state.do_time && (m_process == TIME)) {
				stringstream ss;
				boost::posix_time::time_facet f(m_format.c_str());
				ss.imbue(locale(out.getloc(), &f));
				ss << m_state.now;
				outWrap(out, ss.str());
				return;
			} else if (m_process == COMPLEX) {
				boost::format f(m_format);
				foreach(const string& v, m_variables) {
					if (has(m_state.variables, v)) {
						f % m_state.variables[v];
					} else {
						throw eautover() << ex_message(boost::str(boost::format("Undefined variable '%s' for multi output.") % v));
					}
				}
				outWrap(out, f.str());
				return;
			} else if (m_process == TIMEVAR) {
				if (has(m_state.variables, m_variables[0])) {
					string stime = lexical_cast<string>(m_state.variables[m_variables[0]]);
					stringstream ss;
					boost::posix_time::time_facet f(m_format.c_str());
					boost::posix_time::ptime tim;
					ss.imbue(locale(out.getloc(), &f));
					if (parseTime(stime, tim)) {					
						ss << tim;
						outWrap(out, ss.str());
					} else {
						throw eautover() << ex_message(boost::str(boost::format("Failed to parse content '%s' of variable '%s' for time output.") % stime % m_variables[0]));											
					}
				} else {
					throw eautover() << ex_message(boost::str(boost::format("Undefined variable '%s' for time output.") % m_variables[0]));					
				}
				return;
			}
			out << m_line << endl;
		} catch (eautover& e) {
			e  << ex_lineno(m_lineno) << ex_line(m_line);
			throw;
		} catch (exception& e) {
			throw eautover() << ex_message(e.what()) << ex_lineno(m_lineno) << ex_line(m_line);
		}
	}
private:
	av_state& m_state;

	string m_line;
	long m_lineno;
	TYPE m_process;
	
	string m_prevar;
	string m_postvar;
	string m_comment;
	
	vector<string> m_variables;
	string m_format;
	
	void parseLine() {
		try {
			if (m_state.verbosity > 2)
				output(m_state, 3, boost::str(boost::format("%03d: %s") % m_lineno % m_line));
			
			size_t found = m_line.rfind(m_state.comment());
			if (found != string::npos) {
				string pre = m_line.substr(0, found);
				string comment = m_line.substr(found + m_state.comment().size(), string::npos);
				output(m_state, 4, boost::str(boost::format("%03d: Detected comment: %s") % m_lineno % comment));
				string avcomment;
				if (parse(comment.c_str(), *space_p >> "av::" >> (+anychar_p)[assign_a(avcomment)], nothing_p).full) {
					output(m_state, 2, boost::str(boost::format("%03d: autover comment: %s") % m_lineno % avcomment));
					
					vector<string> tokens;
					if (!parseComment(avcomment, tokens)) {
						warn(m_state, boost::str(boost::format("Unknown / broken autover comment '%s'.") % comment));
						return;
					}
					
					if (m_state.verbosity >= 100) {
						foreach(string& t, tokens)
							cerr << "----> autover comment token *)" << t << "(*" << endl;
					}
						
					if (parseTokens(tokens)) {
						string prevar;
						string var;
						string postvar;
						chset<> strdelim(m_state.string_delim().c_str());
						chset<> linedelim(m_state.line_delim().c_str());
						rule<> any_not_strdelim = anychar_p - strdelim;
						rule<> r_var_str = ((+any_not_strdelim) >> strdelim)[assign_a(prevar)] >> (+any_not_strdelim)[assign_a(var)] >> (strdelim >> *anychar_p)[assign_a(postvar)];
						rule<> end_parser = linedelim;
						rule<> begin_parser;
						begin_parser = anychar_p >> (begin_parser | space_p);
						rule<> r_var_nostr = (begin_parser)[assign_a(prevar)] >> (+(anychar_p - end_parser))[assign_a(var)] >> (*end_parser)[assign_a(postvar)];
						if (!parse(pre.c_str(), r_var_str, nothing_p).full) {
							string trimmedpre = boost::trim_right_copy(pre);
							if (parse(trimmedpre.c_str(), r_var_nostr, nothing_p).full) {
								postvar += pre.substr(trimmedpre.size());
							}
						}
						output(m_state, 5, boost::str(boost::format("*)%s(*") % prevar));
						output(m_state, 5, boost::str(boost::format("*)%s(*") % var));
						output(m_state, 5, boost::str(boost::format("*)%s(*") % postvar));
						
						m_prevar = prevar;
						m_postvar = postvar;
						m_comment = comment;
						
						if ((m_process == VARIABLE) && !has(m_state.variables, m_variables[0])) {
							m_state.variables[m_variables[0]] = var;
						}
					}
				}
			} else {
				output(m_state, 101, "No comment found");
			}
		} catch (eautover& e) {
			e  << ex_lineno(m_lineno) << ex_line(m_line);
			throw;
		} catch (exception& e) {
			throw eautover() << ex_message(e.what()) << ex_lineno(m_lineno) << ex_line(m_line);
		}
	}
	bool parseTokens(const vector<string>& tokens) {
		bool do_parse = false;
		if (tokens[0] == TOKEN_VARIABLE) {
			do_parse = true;
			m_process = VARIABLE;
			m_variables.push_back(tokens[1]);
			if (tokens.size() > 2) {
				m_format = tokens[2];
			} else {
				m_format = "%1%";
			}
		} else if (tokens[0] == TOKEN_DATE) {
			do_parse = true;
			m_process = DATE;
			m_format = tokens[1];
		} else if (tokens[0] == TOKEN_TIME) {
			do_parse = true;
			m_process = TIME;
			m_format = tokens[1];
		} else if (tokens[0] == TOKEN_COMPLEX) {
			do_parse = true;
			m_process = COMPLEX;
			for(size_t i = 1; i < tokens.size()-1; ++i) {
				m_variables.push_back(tokens[i]);
			}
			m_format = tokens[tokens.size()-1];
		} else if (tokens[0] == TOKEN_TIMEVAR) {
			do_parse = true;
			m_process = TIMEVAR;
			m_variables.push_back(tokens[1]);
			m_format = tokens[2];
		} else if (tokens[0] == TOKEN_SET) {
			string setting = tokens[tokens.size() - 2]; // Due to the spirit right-recursion, second to last contains the full setting
			setting = setting.substr(0, setting.size()-1); // cut :
			m_state.settings[setting] = tokens[tokens.size() - 1];
		}
		return do_parse;
	};
	
	void outWrap(ostream& out, const string& str) const {
		out << m_prevar;
		out << str;
		out << m_postvar << m_state.comment() << m_comment << endl;
	};
};

class externalVar {
public:
	externalVar(const string& var, const string& fmt, av_state& state): 
		m_var(var), m_fmt(fmt, boost::regex::extended), m_state(state)
	{
		if (!has(m_state.settings, string("ext:regexno:") + m_var))
			throw eautover() << ex_message(boost::str(boost::format("External variable '%s' misses regexno setting.") % m_var));
		
		m_no = lexical_cast<int>(m_state.settings[string("ext:regexno:") + m_var]);
		output(m_state, 5, "External variable: ", m_var);
		output(m_state, 5, "  Regex: ", fmt);
		output(m_state, 5, boost::str(boost::format("  Regexno: %d") % m_no));
	}
	
	void match(const string& line) const {
		boost::smatch m;
		if (boost::regex_match(line, m, m_fmt)) {
			if (m_no >= m.size())
				throw eautover() << ex_message(boost::str(boost::format("Regex for external variable '%s' matched, but regexno was too big.") % m_var));
			m_state.variables[m_var] = m[m_no];
			output(m_state, 5, boost::str(boost::format("External variable '%s' matches, new value: %s") % m_var % m[m_no]));
		}
	}
private:
	string m_var;
	boost::regex m_fmt;
	av_state& m_state;
	int m_no;
};

void usage(const po::options_description& opts) {
	cout << "Usage: " APPNAME " <options> source file" << endl << endl;
	cout << opts << "Options denoted with a (+) can be given multiple times.\n\n";
}

void process(av_state& state) {
	try {
		if (state.do_external) {
			string cmdline = lexical_cast<string>(state.settings["ext:command"]);
			boost::format cmd(cmdline);
			cmd.exceptions(boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit ));
			string param = lexical_cast<string>(state.settings["ext:parameter"]);
			if (lexical_cast<long>(state.settings["ext:parameter:path"])) {
				fs::path pparam(param);
				if (!pparam.is_complete()) {
					fs::path ppath(fs::initial_path());
					
					ppath /= param;
					ppath = fs::system_complete(ppath);
					param = ppath.file_string();
				}
			}
			cmd % param;
			cmdline = cmd.str();
			
			list<string> outputl;
			{
				boost::shared_ptr<FILE> f(popen(cmdline.c_str(), "r"), pclose);
				if (!f)
					throw eautover() << ex_message(boost::str(boost::format("Failed to execute external command '%s'.") % cmdline));
					
				char buf[256];
				string cur_line;
					
				while (fgets(buf, sizeof (buf), f.get())) {
					cur_line += buf;
					if (cur_line[cur_line.size() - 1] == '\n') {
						outputl.push_back(cur_line);
						cur_line = "";
					}
				}
				if (cur_line != "")
					outputl.push_back(cur_line);
			}
			if (state.verbosity >= 5) {
				output(state, 5, "External command output:");
				int l(0);
				foreach(const string& o, outputl)
					output(state, 5, boost::str(boost::format("%03d: %s") % (++l) % o), "", false);
			}
			
			// Parse external variables
			list<externalVar> exvars;
			foreach(const av_map_pair& v, state.settings) {
				if (boost::algorithm::starts_with(v.first, "ext:regex:")) {
					exvars.push_back(externalVar(v.first.substr(10, string::npos), lexical_cast<string>(v.second), state));
				}
			}
			
			foreach(const string& o, outputl)
				foreach(const externalVar& v, exvars)
					v.match(o);
		}
	} catch (eautover& e) {
		e  << ex_module("Process, external");
		throw;
	} catch (exception& e) {
		throw eautover() << ex_message(e.what()) << ex_module("Process, external");
	}
	
	try {
		foreach(const string& c, state.counters) {
			if (has(state.variables, c)) {
				try {
					long l = lexical_cast<long>(state.variables[c]);
					++l;
					state.variables[c] = l;
				} catch (boost::bad_lexical_cast& e) {
					throw eautover() << ex_message(boost::str(boost::format("Failed to cast value '%s' of variable '%s' to long for counting.") % state.variables[c] % c));
				}
			} else {
				warn(state, boost::str(boost::format("Warning: Requested to count up unknown variable '%s'.") % c));
			}
		}
	} catch (eautover& e) {
		e  << ex_module("Process, counter");
		throw;
	} catch (exception& e) {
		throw eautover() << ex_message(e.what()) << ex_module("Process, counter");
	}
}

void parseFile(const string& filename, av_state& state, bool settings_only = false) {
	try {
		std::list<lineParser> lines;
		
		try {
			string line;
			ifstream reader(filename.c_str());
			
			if (!reader.is_open())
				throw eautover() << ex_message("file open error");
				
			unsigned int lineno = 0;
			while(!reader.eof()) {
				getline(reader, line);
				lines.push_back(lineParser(line, ++lineno, state));
			}
		} catch (eautover& e) {
			e  << ex_module("Read");
			throw;
		} catch (exception& e) {
			throw eautover() << ex_message(e.what()) << ex_module("Read");
		}
		
		if (settings_only)
			return;
			
		process(state);
		
		try {
			stringstream out;
			foreach(const lineParser& l, lines)
				l.writeLine(out);
			
			if (state.do_update) {
				fs::path f_fr(filename);
				fs::path f_to(filename);
				f_to.replace_extension(f_fr.extension() + ".bak");
				fs::remove(f_to);
				fs::rename(f_fr, f_to);
				fs::ofstream w;
				w.open(f_fr);
				w << out.str();
			} else {
				cout << out.str();
			}
		} catch (eautover& e) {
			e  << ex_module("Write");
			throw;
		} catch (exception& e) {
			throw eautover() << ex_message(e.what()) << ex_module("Write");
		}
	} catch (eautover& e) {
		e  << ex_file(filename);
		throw;
	}
}

int main(int argc, char **argv)
{
	vector<string> external_settings;
	string input_file;
	av_state state;
	
	po::options_description avopts("Allowed command-line options");
	avopts.add_options()
		("help,h", "show this help message")
		("date,d", "update date information")
		("time,t", "update time information")
		("external,e", "update information from external program")
		("pre,1", "update pre-build counter")
		("post,2", "update post-build counter")
		("counter,c", po::value<vector<string> >(&state.counters)->composing(), "update arbitrary counter variable (+)")
		("update,u", "write values to original file instead of stdout")
		("verbosity,v", po::value<unsigned int>(&state.verbosity),"set output verbosity level\n"
																  "  0    \tno output (except on errors)\n"
																  "  1    \twarnings only (default)\n"
																  "  2    \tshow detected autover comments\n"
																  "  3    \tshow input lines\n"
																  "  4    \tshow all detected comments\n"
																  "  5    \tshow split elements of value detection and output of the external command\n"
																  "  100  \tshow autover debugging output\n"
		)
		("debug", "shortcut for '--date --time --pre --post --verbosity 50'. The verbosity level of 50 "
				  "is only set if no other is given on the command line. '--external' is not a part of "
				  "this set as it requires an external program (failure to execute is an error).")
		("settings,s", po::value<vector<string> >(&external_settings)->composing(), "load settings from file (+)")
		("strict", "work in strict mode, all warnings are treated as errors.")
	;
	
	po::options_description allopts;
	allopts.add(avopts);
	allopts.add_options()
		("input-file", po::value<string>(&input_file))
	;
	
	po::positional_options_description p;
	p.add("input-file", 1);

	try {

		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).
				  options(allopts).positional(p).run(), vm);
		po::notify(vm);

		if (vm.count("help")) {
			usage(avopts);
			return 1;
		}
		
		if (!vm.count("input-file"))
			throw eautover() << ex_message("source file missing.") << ex_usage(true);
			
		if (vm.count("pre"))
			state.counters.push_back("pre");
		if (vm.count("post"))
			state.counters.push_back("post");
			
		state.do_date = vm.count("date");
		state.do_time = vm.count("time");
		state.do_external = vm.count("external");
		state.strict_mode = vm.count("strict");
		state.do_update = vm.count("update");
		
		if (vm.count("debug")) {
			if (!vm.count("pre")) {
				state.counters.push_back("pre");
			}
			if (!vm.count("post")) {
				state.counters.push_back("post");
			}
			state.do_date = true;
			state.do_time = true;
			if (!vm.count("verbosity")) {
				state.verbosity = 50;
			}
		}
		foreach(const string &s, external_settings) {
			parseFile(s, state, true);
		}
		
		parseFile(input_file, state);

	} catch (eautover& e) {
		boost::shared_ptr<bool const> do_usage = boost::get_error_info<ex_usage>(e);
		if( do_usage && *do_usage )
			usage(avopts);
		cerr << "Error";
		if( boost::shared_ptr<string const> f=boost::get_error_info<ex_module>(e) ) {
			cerr << "[" << *f << "]";
		}
		cerr << ": ";
		
		if( boost::shared_ptr<string const> m=boost::get_error_info<ex_message>(e) ) {
            cerr << *m << endl;
		} else {
			cerr << e.what() << endl;
		}
		if( boost::shared_ptr<string const> f=boost::get_error_info<ex_file>(e) ) {
			cerr << "File: '" << *f << "'" << endl;
		}
		boost::shared_ptr<string const> ls=boost::get_error_info<ex_line>(e);
		boost::shared_ptr<long const> ln=boost::get_error_info<ex_lineno>(e);
		if( ls || ln ) {
			cerr << "Line: ";
			if (ln) {
				cerr << *ln;
				if (ls)
					cerr << ": ";
			}
			if (ls)
				cerr << *ls;
			cerr << endl;
		}
		return -1;
	} catch (exception& e) {
		usage(avopts);
		cerr << "Error: " << e.what() << endl;
		return -1;
	}
	return 0;
}
