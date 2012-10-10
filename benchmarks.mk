# A few common Makefile items

CC = gcc
CXX = g++

UNAME = $(shell uname)

LIB_NAME = model
LIB_SO = lib$(LIB_NAME).so

BASE = ../..
INCLUDE = -I$(BASE)/include  -I$(BASE) -I../include
CPPFLAGS += $(INCLUDE)
CFLAGS += $(INCLUDE)
LDFLAGS += -L$(BASE) -l$(LIB_NAME)

# Mac OSX options
ifeq ($(UNAME), Darwin)
MACFLAGS = -D_XOPEN_SOURCE -DMAC
CPPFLAGS += $(MACFLAGS)
CFLAGS += $(MACFLAGS)
LDFLAGS += $(MACFLAGS)
endif
