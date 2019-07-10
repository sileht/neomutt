/**
 * @file
 * Representation of a mailbox
 *
 * @authors
 * Copyright (C) 1996-2000,2010,2013 Michael R. Elkins <me@mutt.org>
 * Copyright (C) 2018 Richard Russon <rich@flatcap.org>
 *
 * @copyright
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MUTT_MAILBOX_H
#define MUTT_MAILBOX_H

#include <stdbool.h>
#include <sys/types.h>
#include <time.h>
#include "mutt/mutt.h"
#include "config.h"
#include <stdint.h>

struct Email;
struct stat;

#define MB_NORMAL 0
#define MB_HIDDEN 1

/**
 * enum MailboxType - Supported mailbox formats
 */
enum MailboxType
{
  MUTT_MAILBOX_ANY = -2,   ///< Match any Mailbox type
  MUTT_MAILBOX_ERROR = -1, ///< Error occurred examining Mailbox
  MUTT_UNKNOWN = 0,        ///< Mailbox wasn't recognised
  MUTT_MBOX,               ///< 'mbox' Mailbox type
  MUTT_MMDF,               ///< 'mmdf' Mailbox type
  MUTT_MH,                 ///< 'MH' Mailbox type
  MUTT_MAILDIR,            ///< 'Maildir' Mailbox type
  MUTT_NNTP,               ///< 'NNTP' (Usenet) Mailbox type
  MUTT_IMAP,               ///< 'IMAP' Mailbox type
  MUTT_NOTMUCH,            ///< 'Notmuch' (virtual) Mailbox type
  MUTT_POP,                ///< 'POP3' Mailbox type
  MUTT_COMPRESSED,         ///< Compressed file Mailbox type
};

/**
 * enum MailboxNotification - Notifications about changes to a Mailbox
 */
enum MailboxNotification
{
  MBN_CLOSED = 1, ///< Mailbox was closed
  MBN_INVALID,    ///< Email list was changed
  MBN_RESORT,     ///< Email list needs resorting
  MBN_UPDATE,     ///< Update internal tables
  MBN_UNTAG,      ///< Clear the 'last-tagged' pointer
};

/**
 * ACL Rights - These show permission to...
 */
typedef uint16_t AclFlags;          ///< Flags, e.g. #MUTT_ACL_ADMIN
#define MUTT_ACL_NO_FLAGS       0   ///< No flags are set
#define MUTT_ACL_ADMIN   (1 <<  0)  ///< Administer the account (get/set permissions)
#define MUTT_ACL_CREATE  (1 <<  1)  ///< Create a mailbox
#define MUTT_ACL_DELETE  (1 <<  2)  ///< Delete a message
#define MUTT_ACL_DELMX   (1 <<  3)  ///< Delete a mailbox
#define MUTT_ACL_EXPUNGE (1 <<  4)  ///< Expunge messages
#define MUTT_ACL_INSERT  (1 <<  5)  ///< Add/copy into the mailbox (used when editing a message)
#define MUTT_ACL_LOOKUP  (1 <<  6)  ///< Lookup mailbox (visible to 'list')
#define MUTT_ACL_POST    (1 <<  7)  ///< Post (submit messages to the server)
#define MUTT_ACL_READ    (1 <<  8)  ///< Read the mailbox
#define MUTT_ACL_SEEN    (1 <<  9)  ///< Change the 'seen' status of a message
#define MUTT_ACL_WRITE   (1 << 10)  ///< Write to a message (for flagging or linking threads)

#define MUTT_ACL_ALL    ((1 << 11) - 1)

/**
 * struct Mailbox - A mailbox
 */
struct Mailbox
{
  struct Buffer *pathbuf;
  char *realpath; ///< used for duplicate detection, context comparison, and the sidebar
  char *desc;
  off_t size;
  bool has_new; /**< mailbox has new mail */

  /* These next three are only set when MailCheckStats is set */
  int msg_count;             /**< total number of messages */
  int msg_unread;            /**< number of unread messages */
  int msg_flagged;           /**< number of flagged messages */
  int msg_new;               /**< number of new messages */
  int msg_deleted;           /**< number of deleted messages */
  int msg_tagged;            /**< how many messages are tagged? */

  struct Email **emails;
  int email_max;               /**< number of pointers in emails */
  int *v2r;                 /**< mapping from virtual to real msgno */
  int vcount;               /**< the number of virtual messages */

  bool notified;             /**< user has been notified */
  enum MailboxType magic;    /**< mailbox type */
  bool newly_created;        /**< mbox or mmdf just popped into existence */
  struct timespec mtime;
  struct timespec last_visited;       /**< time of last exit from this mailbox */
  struct timespec stats_last_checked; /**< mtime of mailbox the last time stats where checked. */

  const struct MxOps *mx_ops;

  bool append                 : 1; /**< mailbox is opened in append mode */
  bool changed                : 1; /**< mailbox has been modified */
  bool dontwrite              : 1; /**< don't write the mailbox on close */
  bool first_check_stats_done : 1; /**< true when the check have been done at least on time */
  bool peekonly               : 1; /**< just taking a glance, revert atime */
  bool quiet                  : 1; /**< inhibit status messages? */
  bool readonly               : 1; /**< don't allow changes to the mailbox */

  AclFlags rights; /**< ACL bits, see #AclFlags */

#ifdef USE_COMPRESSED
  void *compress_info; /**< compressed mbox module private data */
#endif

  struct Hash *id_hash;     /**< hash table by msg id */
  struct Hash *subj_hash;   /**< hash table by subject */
  struct Hash *label_hash;  /**< hash table for x-labels */

  struct Account *account;
  int opened;              /**< number of times mailbox is opened */

  int flags; /**< e.g. #MB_NORMAL */

  void *mdata;                 /**< driver specific data */
  void (*free_mdata)(void **); /**< driver-specific data free function */

  struct Notify *notify;        ///< Notifications handler
};

/**
 * struct MailboxNode - List of Mailboxes
 */
struct MailboxNode
{
  struct Mailbox *mailbox;
  STAILQ_ENTRY(MailboxNode) entries;
};
STAILQ_HEAD(MailboxList, MailboxNode);

/**
 * struct EventMailbox - An Event that happened to a Mailbox
 */
struct EventMailbox
{
  struct Mailbox *mailbox; ///< The Mailbox this Event relates to
};

/**
 * enum NotifyMailbox - Types of Mailbox Event
 */
enum NotifyMailbox
{
  NT_MAILBOX_ADD = 1, ///< A new Mailbox has been created
  NT_MAILBOX_REMOVE,  ///< A Mailbox is about to be destroyed
};

void            mailbox_free             (struct Mailbox **ptr);
struct Mailbox *mailbox_new              (void);
void            mutt_mailbox_changed     (struct Mailbox *m, enum MailboxNotification action);
void            mutt_mailbox_cleanup     (const char *path, struct stat *st);
struct Mailbox *mutt_mailbox_find        (const char *path);
struct Mailbox *mutt_mailbox_find_desc   (const char *desc);
void            mutt_mailbox_size_add    (struct Mailbox *m, const struct Email *e);
void            mutt_mailbox_size_sub    (struct Mailbox *m, const struct Email *e);
void            mutt_mailbox_update      (struct Mailbox *m);

#endif /* MUTT_MAILBOX_H */
