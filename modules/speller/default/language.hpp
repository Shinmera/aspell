// Copyright 2004 by Kevin Atkinson under the terms of the LGPL

#ifndef ASPELLER_LANGUAGE__HPP
#define ASPELLER_LANGUAGE__HPP

#include "affix.hpp"
#include "cache.hpp"
#include "config.hpp"
#include "convert.hpp"
#include "phonetic.hpp"
#include "posib_err.hpp"
#include "stack_ptr.hpp"
#include "string.hpp"
#include "objstack.hpp"

using namespace acommon;

namespace acommon {
  struct CheckInfo;
}

namespace aspeller {

  struct SuggestRepl {
    const char * substr;
    const char * repl;
  };
  
  class SuggestReplEnumeration
  {
    const SuggestRepl * i_;
    const SuggestRepl * end_;
  public:
    SuggestReplEnumeration(const SuggestRepl * b, const SuggestRepl * e)
      : i_(b), end_(e) {}
    bool at_end() const {return i_ == end_;}
    const SuggestRepl * next() {
      if (i_ == end_) return 0;
      return i_++;
    }
  };

  // WordInfo

  typedef unsigned int WordInfo; // 4 bits

  enum CasePattern {Other, FirstUpper, AllLower, AllUpper};
  //   Other      00
  //   FirstUpper 01
  //   AllLower   10
  //   AllUpper   11
  // First bit : is upper
  // Second bit: uniform case

  static const WordInfo CASE_PATTERN = 3;
  static const WordInfo ALL_PLAIN    = (1 << 2);
  static const WordInfo ALL_LETTER   = (1 << 3);

  // CharInfo

  typedef unsigned int CharInfo; // 5 bits

  static const CharInfo LOWER  = (1 << 0);
  static const CharInfo UPPER  = (1 << 1);
  static const CharInfo TITLE  = (1 << 2);
  static const CharInfo PLAIN  = (1 << 3);
  static const CharInfo LETTER = (1 << 4);

  static const CharInfo CHAR_INFO_ALL = 0x1F;

  //

  enum StoreAs {Stripped, Lower};

  class Language : public Cacheable {
  public:
    typedef const Config CacheConfig;
    typedef String       CacheKey;

    enum CharType {Unknown, WhiteSpace, Hyphen, Digit, 
                   NonLetter, Modifier, Letter};
    
    struct SpecialChar {
      bool begin;
      bool middle;
      bool end;
      bool any;
      SpecialChar() : begin(false), middle(false), end(false), any(false) {}
      SpecialChar(bool b, bool m, bool e) : begin(b), middle(m), end(e),
                                            any(b || m || e) {}
    };

  private:
    String   dir_;
    String   name_;
    String   charset_;
    String   data_encoding_;

    ConvObj  mesg_conv_;
    ConvObj  to_utf8_;
    ConvObj  from_utf8_;

    unsigned char to_uchar(char c) const {return static_cast<unsigned char>(c);}

    SpecialChar special_[256];
    CharInfo      char_info_[256];
    char          to_lower_[256];
    char          to_upper_[256];
    char          to_title_[256];
    char          to_stripped_[256];
    unsigned char to_normalized_[256];
    char          to_plain_[256];
    int           to_uni_[256];
    CharType      char_type_[256];
    char *        to_clean_;

    StoreAs       store_as_;

    int max_normalized_;

    String      soundslike_chars_;
    String      clean_chars_;

    StackPtr<Soundslike> soundslike_;
    StackPtr<AffixMgr>   affix_;
    StackPtr<Config>     lang_config_;

    StringBuffer buf_;
    Vector<SuggestRepl> repls_;

    Language(const Language &);
    void operator=(const Language &);

  public: // but don't use

    char          sl_first_[256];
    char          sl_rest_[256];

  public:

    Language() {}
    PosibErr<void> setup(const String & lang, const Config * config);
    void set_lang_defaults(Config & config);

    const char * data_dir() const {return dir_.c_str();}
    const char * name() const {return name_.c_str();}
    const char * charset() const {return charset_.c_str();}
    const char * data_encoding() const {return data_encoding_.c_str();}

    const Convert * mesg_conv() const {return mesg_conv_.ptr;}
    const Convert * to_utf8() const {return to_utf8_.ptr;}
    const Convert * from_utf8() const {return from_utf8_.ptr;}

    char to_upper(char c) const {return to_upper_[to_uchar(c)];}
    bool is_upper(char c) const {return to_upper(c) == c;}

    char to_lower(char c) const {return to_lower_[to_uchar(c)];}
    bool is_lower(char c) const {return to_lower(c) == c;}

    char to_title(char c) const {return to_title_[to_uchar(c)];}
    bool is_title(char c) const {return to_title(c) == c;}

    char to_stripped(char c) const {return to_stripped_[to_uchar(c)];}
    bool is_stripped(char c) const {return to_stripped(c) == c;}

    char to_normalized(char c) const {return to_normalized_[to_uchar(c)];}
    unsigned char max_normalized() const {return max_normalized_;}
    
    char to_plain(char c) const {return to_plain_[to_uchar(c)];}

    char to_clean(char c) const {return to_clean_[to_uchar(c)];}
    bool is_clean(char c) const {return to_clean(c) == c;}
  
    int to_uni(char c) const {return to_uni_[to_uchar(c)];}
  
    SpecialChar special(char c) const {return special_[to_uchar(c)];}
  
    CharType char_type(char c) const {return char_type_[to_uchar(c)];}
    bool is_alpha(char c) const {return char_type(c) >  NonLetter;}

    CharInfo char_info(char c) const {return char_info_[to_uchar(c)];}
  
    const char * to_soundslike(String & res, ParmString word) const {
      res.resize(word.size());
      char * e = soundslike_->to_soundslike(res.data(), word.str(), word.size());
      res.resize(e - res.data());
      return res.str();
    }

    const char * soundslike_name() const {
      return soundslike_->name();
    }
    bool have_soundslike() const {return strcmp(soundslike_->name(),"none") == 0;}

    const char * soundslike_version() const {
      return soundslike_->version();
    }
    
    const char * soundslike_chars() const {return soundslike_chars_.c_str();}
    const char * clean_chars() const {return clean_chars_.c_str();}

    const AffixMgr * affix() const {return affix_;}

    bool have_affix() const {return affix_;}

    SuggestReplEnumeration * repl() const {
      return new SuggestReplEnumeration(repls_.pbegin(), repls_.pend());}

    char * to_soundslike(char * res, const char * str, int len = -1) const 
    { return soundslike_->to_soundslike(res,str,len);}

    void munch(ParmString word, CheckList * cl) const {affix_->munch(word, cl);}

    WordAff * expand(ParmString word, ParmString aff, 
                     ObjStack & buf, int limit = INT_MAX) const {
      return affix_->expand(word, aff, buf, limit);
    }
    
    char * to_lower(char * res, const char * str) const {
      while (*str) *res++ = to_lower(*str++); *res = '\0'; return res;}
    char * to_upper(char * res, const char * str) const {
      while (*str) *res++ = to_upper(*str++); *res = '\0'; return res;}
    char * to_stripped(char * res, const char * str) const {
      for (; *str; ++str) {
        char c = to_stripped(*str);
        if (c) *res++ = c;
      }
      *res = '\0';
      return res;
    }
    char * to_clean(char * res, const char * str) const {
      for (; *str; ++str) {
        char c = to_clean(*str);
        if (c) *res++ = c;
      }
      *res = '\0';
      return res;
    }

    const char * to_lower(String & res, const char * str) const {
      res.clear(); while (*str) res += to_lower(*str++); return res.str();}
    const char * to_upper(String & res, const char * str) const {
      res.clear(); while (*str) res += to_upper(*str++); return res.str();}
    const char * to_stripped(String & res, const char * str) const {
      res.clear();
      for (; *str; ++str) {
        char c = to_stripped(*str);
        if (c) res += c;
      }
      return res.str();
    }
    const char * to_clean(String & res, const char * str) const {
      res.clear();
      for (; *str; ++str) {
        char c = to_clean(*str);
        if (c) res += c;
      }
      return res.str();
    }

    bool is_lower(const char * str) const {
      while (*str) {if (!is_lower(*str++)) return false;} return true;}
    bool is_upper(const char * str) const {
      while (*str) {if (!is_upper(*str++)) return false;} return true;}
    bool is_stripped(const char * str) const {
      while (*str) {if (!is_stripped(*str++)) return false;} return true;}
    bool is_clean(const char * str) const {
      while (*str) {if (!is_clean(*str++)) return false;} return true;}

    WordInfo get_word_info(ParmString str) const;
    CasePattern case_pattern(ParmString str) const;

    void fix_case(CasePattern case_pattern, char * str)
    {
      if (!str[0]) return;
      if (case_pattern == AllUpper) to_upper(str,str);
      else if (case_pattern == FirstUpper) *str = to_title(*str);
    }
    void fix_case(CasePattern case_pattern, 
                  char * res, const char * str) const;
    const char * fix_case(CasePattern case_pattern, 
                          const char * str, String buf) const;

    static inline PosibErr<Language *> get_new(const String & lang, const Config * config) {
      StackPtr<Language> l(new Language());
      RET_ON_ERR(l->setup(lang, config));
      return l.release();
    }

    bool cache_key_eq(const String & l) const  {return name_ == l;}
  };

  typedef Language LangImpl;

  struct MsgConv : public ConvP
  {
    MsgConv(const Language * l) : ConvP(l->mesg_conv()) {}
    MsgConv(const Language & l) : ConvP(l.mesg_conv()) {}
  };

  struct InsensitiveCompare {
    // compares to strings without regards to casing or special characters
    const Language * lang;
    InsensitiveCompare(const Language * l = 0) : lang(l) {}
    operator bool () const {return lang;}
    int operator() (const char * a, const char * b) const
    { 
      char x,y;
      for (;;)
      {
        while (x = lang->to_clean(*a++), !x);
        while (y = lang->to_clean(*b++), !y);
        if (x == 1 || y == 1 || x == y) break;
      }
      return static_cast<unsigned char>(x) - static_cast<unsigned char>(y);
    }
  };

  struct InsensitiveEqual {
    InsensitiveCompare cmp;
    InsensitiveEqual(const Language * l = 0) : cmp(l) {}
    bool operator() (const char * a, const char * b) const
    {
      return cmp(a,b) == 0;
    }
  };
  
  struct InsensitiveHash {
    // hashes a string without regards to casing or special begin
    // or end characters
    const Language * lang;
    InsensitiveHash() {}
    InsensitiveHash(const Language * l)
	: lang(l) {}
    size_t operator() (const char * s) const
    {
      size_t h = 0;
      for (;;) {
	if (*s == 0) break;
        char c = lang->to_clean(*s++);
	if (c) h=5*h + lang->to_clean(*s);
      }
      return h;
    }
  };

  struct SensitiveCompare {
    const Language * lang;
    bool case_insensitive;
    bool ignore_accents;
    bool strip_accents;
    SensitiveCompare(const Language * l = 0) 
      : lang(l), case_insensitive(false)
      , ignore_accents(false), strip_accents(false) {}
    bool operator() (const char * word, const char * inlist) const;
    // Pre:
    //   word == to_find as given by Language::InsensitiveEqual 
    //   both word and inlist contain at least one letter as given by
    //     lang->char_type
    // Rules:
    //   if begin inlist is a begin char then it must match begin word
    //   if end   inlist is a end   char then it must match end word
    //   chop all begin/end chars from the begin/end of word and inlist
    //  unless ignore_accents
    //   accents must match
    //  unless case_insensitive
    //   (note: there are 3 posssible casings lower, upper and title)
    //   if is lower begin inlist then begin word can be any casing
    //   if not                   then begin word must be the same case
    //   if word is all upper than casing of inlist can be anything
    //   otherwise the casing of tail begin and tail inlist must match
  };

  struct ConvertWord {
    const Language * lang;
    bool strip_accents;
    ConvertWord(const Language * l = 0)
      : lang(l), strip_accents(false) {}
    void convert(ParmString in, String & out) const
    {
      if (!strip_accents) {
	out += in;
      } else {
	for (unsigned int i = 0; i != in.size(); ++i)
	  out += lang->to_plain(in[i]);
      }
    }
    void convert(ParmString in, char * out) const
    {
      if (!strip_accents) {
        memcpy(out, in, in.size() + 1);
      } else {
        unsigned int i = 0;
	for (; i != in.size(); ++i)
	  out[i] = lang->to_plain(in[i]);
        out[i] = '\0';
      }
    }
  };

  String get_stripped_chars(const Language & l);

  String get_clean_chars(const Language & l);
  
  PosibErr<void> check_if_valid(const Language & l, ParmString word);

  PosibErr<Language *> new_language(const Config &, ParmString lang = 0);

  PosibErr<void> open_affix_file(const Config &, FStream & o);
  
}


#endif
