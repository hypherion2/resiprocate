#include <iostream>

#include "resiprocate/os/Data.hxx"
#include "resiprocate/Uri.hxx"

using namespace resip;
using namespace std;

class GagMessage
{
  public:
    typedef enum
    {
      // GAIM <-> GAG
      IM                 = 0x00,
      PRESENCE           = 0x01,
  
      // GAIM --> GAG
      LOGIN              = 0x40,
      LOGOUT             = 0x41,
      ADD_BUDDY          = 0x42,
      REMOVE_BUDDY       = 0x43,

      // GAG --> GAIM
      ERROR              = 0x80

    } command_t;

  public:
    static GagMessage *getMessage(istream &is);
    bool isValid() {return valid;}
    virtual ostream &serialize(ostream &os) const = 0;

  protected: // methods
    virtual void parse(istream &is) = 0;
    GagMessage(){valid = true;}


    static bool parse(istream &, Data &);
    static bool parse(istream &, Uri &);
    static bool parse(istream &, bool &);

    static void serialize(ostream &, const Data &);
    static void serialize(ostream &, const Uri &);
    static void serialize(ostream &, const bool &);

  protected: // attributes
    command_t messageType;
    bool valid;
};

class GagImMessage : public GagMessage
{
  public:
    GagImMessage(Uri &_from, Uri &_to, Data &_im) :
      from(_from), to(_to), im(_im) {messageType = IM;}

    GagImMessage(istream &is) { messageType = IM; parse (is); }

    virtual ostream &serialize(ostream &os) const;
    virtual void parse(istream &is);
  private:
    Uri from;
    Uri to;
    Data im;
};

class GagPresenceMessage : public GagMessage
{
  public:
    GagPresenceMessage(Uri &_aor, bool _available, Data &_status):
      aor(_aor), available(_available), status(_status)
      { messageType=PRESENCE ;}

    GagPresenceMessage(istream &is) { messageType=PRESENCE; parse (is); }

    virtual ostream &serialize(ostream &os) const;
    virtual void parse(istream &is);
  private:
    Uri aor;
    bool available;
    Data status;
};

class GagLoginMessage : public GagMessage
{
  public:
    GagLoginMessage(Uri &_aor, Data &_userid, Data &_password):
      aor(_aor), userid(_userid), password(_password)
      { messageType=LOGIN; }
    GagLoginMessage(istream &is) { messageType=LOGIN; parse (is); }

    virtual ostream &serialize(ostream &os) const;
    virtual void parse(istream &is);
  private:
    Uri aor;
    Data userid;
    Data password;
};

class GagLogoutMessage : public GagMessage
{
  public:
    GagLogoutMessage(Uri &_aor) : aor(_aor) {messageType=LOGOUT;}
    GagLogoutMessage(istream &is) {messageType=LOGOUT; parse(is); }

    virtual ostream &serialize(ostream &os) const;
    virtual void parse(istream &is);
  private:
    Uri aor;
};

class GagAddBuddyMessage : public GagMessage
{
  public:
    GagAddBuddyMessage(Uri &_buddy) : buddy(_buddy)
      { messageType=ADD_BUDDY; }
    GagAddBuddyMessage(istream &is) { messageType=ADD_BUDDY; parse(is); }

    virtual ostream &serialize(ostream &os) const;
    virtual void parse(istream &is);
  private:
    Uri buddy;
};

class GagRemoveBuddyMessage : public GagMessage
{
  public:
    GagRemoveBuddyMessage(Uri &_buddy) : buddy(_buddy)
      {messageType=REMOVE_BUDDY;}
    GagRemoveBuddyMessage(istream &is) 
      { messageType=REMOVE_BUDDY; parse (is); }

    virtual ostream &serialize(ostream &os) const;
    virtual void parse(istream &is);
  private:
    Uri buddy;
};

class GagErrorMessage : public GagMessage
{
  public:
    GagErrorMessage(Data &_message) : message(_message)
      {messageType=ERROR;}
    GagErrorMessage(istream &is) {messageType=ERROR; parse(is);}

    virtual ostream &serialize(ostream &os) const;
    virtual void parse(istream &is);
  private:
    Data message;
};

inline
ostream &
operator<< (ostream &os, const GagMessage &msg)
{
  return msg.serialize(os);
}
