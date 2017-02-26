/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 *
 * This file is released under the 3-clause BSD License. The full license text
 * can be found in LICENSE in the top-level directory.
 */
namespace redc
{
  // We use fseek/ftell instead of fpos_t because integers can be copied
  // around, I think copying the fpos_t is not necessarily defined.
  struct File_Cursor
  {
    File_Cursor() : pos(0), file(NULL) {}
    // Current position in a file
    long pos;

    // The file we are dealing with, if the file changes we want to know.
    std::FILE* file;
  };

  // Streams one file for multiple users that may be at the file at different
  // points.
  template <class Lock_T = Null_Lock>
  struct File_Stream
  {
    explicit File_Stream(Lock_T lock = Lock_T());
    File_Stream(std::string const& filename, const char* const* mode,
                Lock_T lock = Lock_T());
    ~File_Stream();

    File_Stream(File_Stream&& fs);
    File_Stream& operator=(File_Stream&& fs);

    // Resets all cursors to this new file.
    void load_file(std::string const& filename, const char* const* mode);

    bool valid_file() const
    { return file_ != NULL; }

    bool eof() const;

    void reset_cursor(File_Cursor& cur) const;

    std::string read(File_Cursor& cursor, unsigned int bytes) const;
  private:
    void open_(std::string const& filename, const char* const* mode);
    void close_();

    Lock_T file_lock_;

    long cur_pos_;
    std::FILE* file_;
  };

  template <class Lock_T>
  File_Stream<Lock_T>::File_Stream(Lock_T lock)
    : file_lock_(std::move(lock)), cur_pos_(0), file_(NULL) {}

  template <class Lock_T>
  File_Stream<Lock_T>::File_Stream(std::string const& filename,
                                   const char* const* mode, Lock_T lock)
    : file_lock_(std::move(lock)), cur_pos_(0), file_(NULL)
  {
    open_(filename, mode);
  }

  template <class Lock_T>
  void File_Stream<Lock_T>::initialize_from_(File_Stream&& fs)
  {
    // Lock, since we are likely changing the FILE handle.
    std::lock_guard<Lock_T> raii_lock(fs.file_lock_);
    file_lock_ = std::move(fs.file_lock_);

    cur_pos_ = fs.cur_pos_;
    file_ = fs.file_;

    fs.file_ = nullptr;
  }

  template <class Lock_T>
  File_Stream<Lock_T>::File_Stream(File_Stream&& fs)
  {
    initialize_from_(std::move(fs));
  }
  File_Stream<Lock_T>& File_Stream<Lock_T>::operator=(File_Stream&& fs)
  {
    initialize_from_(std::move(fs));
    return *this;
  }

  template <class Lock_T>
  void File_Stream<Lock_T>::load_file(std::string const& filename, const char* const* mode)
  {
    close_();
    open_(filename, mode);
  }

  template <class Lock_T>
  void File_Stream<Lock_T>::open_(std::string const& filename, const char* const* mode)
  {
    std::lock_guard<Lock_T> raii_lock(file_lock_);

    std::FILE* new_file = std::fopen(filename.c_str(), mode);
    if(!new_file)
    {
      // Shit.
      log_e("Failed to open '%'", filename);
      return;
    }

    file_ = new_file;
    cur_pos_ = 0;
  }
  template <class Lock_T>
  void File_Stream<Lock_T>::close_()
  {
    std::lock_guard<Lock_T> raii_lock(file_lock_);

    if(file_)
    {
      std::fclose(file_);
      file_ = NULL;
    }
  }

  template <class Lock_T>
  File_Stream<Lock_T>::~File_Stream()
  {
    close_();
  }

  template <class Lock_T>
  bool File_Stream<Lock_T>::eof() const
  {
    if(file_ == NULL) return false;
    return (bool) std::feof(file_);
  }

  template <class Lock_T>
  void File_Stream<Lock_T>::reset_cursor(File_Cursor& cursor) const
  {
    cursor.pos = 0;
    cursor.file = file_;
  }

  template <class Lock_T>
  std::string File_Stream<Lock_T>::read(File_Cursor& cursor, unsigned int bytes) const
  {
    // Time to read.
    std::lock_guard raii_lock(file_lock_);

    // If we don't have a file at all file, we have nothing to do, return an
    // empty string.
    if(file_ == NULL) return "";

    // If that cursor is old or for a different file, start from scratch.
    if(cursor.file != file_)
    {
      cursor.file = file_;
      cursor.ret = 0;
    }

    if(cur_pos_ != cursor.pos)
    {
      // Seek to the correct position in the file for this cursor.
      std::fseek(file_, cursor.pos);
      cur_pos_ = cursor.pos;
    }

    // We're in the right position so start reading.
    std::string ret;
    ret.resize(max);
    std::size_t read_len = std::fread(&ret[0], sizeof(char), max, file_);

    // Where do we stand?
    if(feof(file_))
    {
      // End of file
    }

    // It's okay if something failed, we should just return an empty string.
    ret.resize(read_len);

    // Update the position of the input cursor.
    cursor.pos = ftell(file_);
    cur_pos_ = cursor.pos;

    return ret;
  }
}
