/*
    This file is part of MutekH.

    MutekH is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    MutekH is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MutekH; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2009

*/

#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <algorithm>

#include <stdio.h>
#include <dpp/foreach>
#include <dpp/interval_set>

#include <elfpp/object>
#include <elfpp/section>
#include <elfpp/symbol>

#include "args.h"

/**********************************************************************
 *                      Log messages                                  *
 **********************************************************************/

enum disp_level_e {
  disp_fatal,
  disp_error,
  disp_warning,
  disp_notice,
  disp_verbose,
  disp_debug
};

int disp_level = disp_notice;
static char *conf_filename = "hetlink.conf";
static char *filename_suffix = ".het.o";
static int error_count = 0;

#define DISPLAY(l, ...)				\
{						\
  if (l <= disp_error)				\
    { std::cerr << "error:"; error_count++; }	\
  if (l <= disp_level)				\
    std::cerr << __VA_ARGS__ << std::endl;	\
  if (l <= disp_fatal)				\
    exit(1);					\
}

/**********************************************************************
 *                      Command line arguments                        *
 **********************************************************************/

char *args_title_g = "MutekH Heterogeneous object linker";
char *args_copyright_g = "Alexandre Becoulet (C) 2009";
char *args_usage_g = "hetlink [options] object-arch1.o object-arch2.o ...";

struct args_list_s argslist_g[] = {
  { "-h", "--help", "display this help message\n", 0, args_func_call, {(void*)&args_help}, 0, 0 },
  { "-c", "--conf-file", "configuration file name (hetlink.conf)", 1, args_varstr_set, &conf_filename, 0 },
  { "-s", "--output-suffix", "output objects file name suffix (.het.o)", 1, args_varstr_set, &filename_suffix, 0 },
  { "-v", "--verbosity", "verbosity in [0, 5] range (3)", 1, args_varint_set, &disp_level, 0 },
  { 0 }
};

/**********************************************************************
 *                      Het structures                                *
 **********************************************************************/

struct het_object_s
{
  std::string filename_;
  elfpp::object *obj_;
};

struct het_symbol_s
{
  std::string name_;
  struct het_section_s *section_;
  uint32_t value_;
  size_t size_;
  unsigned int ref_count_;
  std::list<elfpp::symbol*> symbols_;

  het_symbol_s() 
    : ref_count_(0), symbols_() {}
};

typedef std::map<std::string, het_symbol_s *> het_symbols_map_t;

typedef dpp::interval_set<uint32_t> sec_alloc_t;

struct het_section_s
{
  std::string name_;
  het_symbols_map_t syms_;
  unsigned int ref_count_;
  sec_alloc_t free_;		// space that can be used to relocate symbols
  char action_;
  std::list<elfpp::section*> sections_;

  het_section_s()
    : ref_count_(0), free_(true) {}
};

typedef std::map<std::string, het_section_s *> het_sections_map_t;


/**********************************************************************
 *                      Het data                                      *
 **********************************************************************/

static std::vector<het_object_s> het_objects;
static het_sections_map_t het_sections;

/**********************************************************************
 *                      Functions                                     *
 **********************************************************************/

static bool match_symbol(const elfpp::symbol &s)
{
  if (s.get_type() == elfpp::STT_SECTION)
    return false;

  if (s.get_size() == 0)
    {
      if (!s.get_reloc_table().empty())
	DISPLAY(disp_error, "Symbol with zero size has relocations: " << s.get_name());
      return false;
    }

  return true;
}

// Load input files and create heterogeneous objects
static void load_objs()
{
  for (int i = 0; args_param_list_g[i]; i++)
    {
      het_object_s ho;
      ho.filename_ = args_param_list_g[i];

      DISPLAY(disp_notice, "Loading: " << ho.filename_);

      ho.obj_ = new elfpp::object(ho.filename_);
      ho.obj_->parse_symbol_table();
      ho.obj_->load_symbol_data();

      het_objects.push_back(ho);
    }
}


// read configuration file and create heterogeneous sections

static void load_conf()
{
  FILE *conf = fopen(conf_filename, "r");
  char buff[256];

  if (!conf)
    DISPLAY(disp_fatal, "Unable to open configuration file: " << conf_filename);

  while (char *line = fgets(buff, 256, conf))
    {
      line += strspn(line, " \n\t");

      if (!*line || *line == '#')
	continue;

      const char *name = strsep(&line, ", \n\t");
      const char *action = strsep(&line, ", \n\t");

      het_section_s *hsec = new het_section_s;
      hsec->name_ = name;
      hsec->action_ = *action;
      het_sections.insert(het_sections_map_t::value_type(hsec->name_, hsec));
    }

  fclose(conf);
}



int main(int argc, char **argv)
{
  if (args_parse(argc - 1, argv + 1) || args_check_mandatory(0))
    return (0);

  load_objs();
  load_conf();

  // Process files

  FOREACH(o, het_objects)
    {
      DISPLAY(disp_notice, "Finding homonyms in: " << o->filename_);

      FOREACH(S, o->obj_->get_section_table())
	{
	  if (!(S->get_flags() & elfpp::SHF_ALLOC))
	    continue;

	  het_sections_map_t::iterator i = het_sections.find(S->get_name());

	  if (i == het_sections.end())
	    continue;

	  DISPLAY(disp_verbose, "  Section: " << S->get_name());

	  het_section_s *hsec = i->second;
	  hsec->ref_count_++;

	  hsec->sections_.push_back(&*S);

	  // default section free space is above current section size
	  sec_alloc_t alloc(S->get_size(), 1<<31);

	  //	  S->set_size(0);

	  // find or create new het-symbols
	  FOREACH(s_, S->get_symbol_table())
	    {
	      elfpp::symbol &s = *s_->second;

	      if (!match_symbol(s))
		continue;

	      het_symbols_map_t::iterator i = hsec->syms_.find(s.get_name());

	      het_symbol_s *hsym;

	      if (i == hsec->syms_.end())
		{
		  // symbol name yet unknown

		  DISPLAY(disp_debug, "    New symbol: " << s.get_name());

		  hsym = new het_symbol_s;
		  hsym->name_ = s.get_name();
		  hsym->section_ = hsec;
		  hsym->size_ = s.get_size();
		  hsec->syms_.insert(het_symbols_map_t::value_type(hsym->name_, hsym));
		}
	      else
		{
		  hsym = &*i->second;

		  FOREACH(os, hsym->symbols_)
		    {
		      if ((*os)->get_section() == s.get_section())
			DISPLAY(disp_error, s.get_name() << " symbol name is ambiguous (multiple static symbols with the same name ?)");
		    }

		  // already existing symbol name
		  DISPLAY(disp_debug, "    Het symbol: " << s.get_name());

		  if (hsym->size_ < s.get_size())
		    hsym->size_ = s.get_size();
		}

	      hsym->symbols_.push_back(&s);
	      hsym->ref_count_++;
	      // add symbol storage to section allocatable space
	      alloc |= sec_alloc_t::interval_type(s.get_value(), true, s.get_value() + s.get_size(), false);
	    }

	  // het section allocatable space is objects sections space intersection
	  hsec->free_ &= alloc;
	}
    }

  FOREACH(hsec_, het_sections)
    {
      het_section_s *HS = hsec_->second;

      if (HS->ref_count_ < 2)
	{
	  DISPLAY(disp_error, HS->name_ << " section is not used in more than one object file");
	  continue;
	}

      DISPLAY(disp_verbose, "Relocatable space in " << HS->name_ << " section: " << std::hex << HS->free_);

      switch (HS->action_)
	{
	  /************************************************************/
	  // reoder all symbols in the section, consider largest one
	case 'c': {
	  uint32_t v = 0;
	  DISPLAY(disp_notice, "Reordering functions in: " << HS->name_);

	  FOREACH(hsym_, HS->syms_)
	    {
	      het_symbol_s *hsym = &*hsym_->second;

	      if (hsym->ref_count_ < het_objects.size())
		DISPLAY(disp_verbose, "  " << hsym->name_ << " not present in all objects");

	      DISPLAY(disp_debug, "  " << hsym->name_ << " moved to " << std::hex << v);
	      FOREACH(s, hsym->symbols_)
		{
		  DISPLAY(disp_debug, "    From " << (*s)->get_value());
		  (*s)->set_value(v);
		}

	      v += hsym->size_;
	    }
	  break;
	}

	  /************************************************************/
	  // reoder all symbols in the section, size and content must match
	case 'd': {
	  uint32_t v = 0;
	  DISPLAY(disp_notice, "Reordering variables in: " << HS->name_);

	  FOREACH(hsym_, HS->syms_)
	    {
	      hsym_->second->value_ = v;
	      v += std::max(hsym_->second->size_, (size_t)4);
	    }

	  FOREACH(S, HS->sections_)
	    (*S)->set_size(v);

	  FOREACH(hsym_, HS->syms_)
	    {
	      het_symbol_s *hsym = &*hsym_->second;

	      ssize_t size = -1;
	      uint8_t *content = NULL;
	      DISPLAY(disp_debug, "  " << hsym->name_ << " moved to " << std::hex << v);

	      // Check that all symbols have coherent content and set equal section offset
	      FOREACH(s, hsym->symbols_)
		{
		  DISPLAY(disp_debug, "    From " << (*s)->get_value());

		  if (size < 0)
		    {
		      size = (*s)->get_size();
		      content = (*s)->get_content();
		    }
		  else
		    {
		      if (size != (*s)->get_size())
			DISPLAY(disp_error, "  " << hsym->name_ << " has different storage sizes");

		      if ((*s)->get_mangling_relocs().empty() &&
			  (*s)->get_section()->get_type() != elfpp::SHT_NOBITS &&
			  memcmp(content, (*s)->get_content(), size))
			DISPLAY(disp_error, "  " << hsym->name_ << " has different values");
		    }

		  (*s)->set_value(hsym->value_);
		}

	      // Copy symbol content to all objects for symbols not present everywhere.
	      // This ensures any single object can be used to load data sections.
	      if (hsym->ref_count_ < het_objects.size())
		{
		  DISPLAY(disp_verbose, "  " << hsym->name_ << " not present in all objects");

		  FOREACH(s, hsym->symbols_)
		    if (!(*s)->get_mangling_relocs().empty())
		      DISPLAY(disp_error, "  " << hsym->name_ << " variable with relocation not present in all objects");

		  FOREACH(S, HS->sections_)
		    if ((*S)->get_type() != elfpp::SHT_NOBITS)
		      memcpy((*S)->get_content() + hsym->value_, content, size);
		}
	    }

	  break;
	}

	};
    }

  if (error_count)
    exit(1);

  FOREACH(ho, het_objects)
    {
      std::string filename(ho->filename_ + std::string(filename_suffix));
      DISPLAY(disp_notice, "Writing " << filename);
      ho->obj_->write(filename);
    }

}

