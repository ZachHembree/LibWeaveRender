#pragma once
#include <iostream>
#include <streambuf>
#include <ios>
#include <span>
#include "WeaveUtils/Span.hpp"

namespace Weave 
{
    /// <summary>
    /// Stream buffer wrapping a non-owning view to a span of characters
    /// </summary>
    class SpanBuffer : public std::streambuf 
    {
    public:
        SpanBuffer(std::span<char> buffer, std::ios_base::openmode mode)
            : mode(mode) 
        {
            setup_pointers(buffer.data(), buffer.size());
        }

        SpanBuffer(Span<char> buffer, std::ios_base::openmode mode)
            : mode(mode)
        {
            setup_pointers(buffer.GetData(), buffer.GetLength());
        }

        SpanBuffer(std::span<const char> buffer, std::ios_base::openmode mode)
            : mode(mode) 
        {
            if (mode & std::ios_base::out) 
            {
                // Cannot have output mode with a const buffer
                // Set no pointers to make the stream immediately invalid
                return;
            }
            // It's generally considered safe to const_cast for std::streambuf's get area
            // as input operations will not modify the buffer.
            char* start = const_cast<char*>(buffer.data());
            setup_pointers(start, buffer.size());
        }

        SpanBuffer(Span<const char> buffer, std::ios_base::openmode mode)
            : mode(mode)
        {
            if (mode & std::ios_base::out)
                return;

            char* start = const_cast<char*>(buffer.GetData());
            setup_pointers(start, buffer.GetLength());
        }

    protected:
        // Override seekoff for relative positioning
        pos_type seekoff(off_type off, std::ios_base::seekdir way, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override 
        {
            if ((which & std::ios_base::in) && (mode & std::ios_base::in)) 
            {
                char* base = eback();
                char* new_pos = gptr();

                if (way == std::ios_base::beg)
                    new_pos = base + off;              
                else if (way == std::ios_base::cur)
                    new_pos = gptr() + off;             
                else if (way == std::ios_base::end)
                    new_pos = egptr() + off;

                if (new_pos >= eback() && new_pos <= egptr()) 
                {
                    setg(eback(), new_pos, egptr());
                    return new_pos - eback();
                }
            }

            if ((which & std::ios_base::out) && (mode & std::ios_base::out)) 
            {
                char* base = pbase();
                char* new_pos = pptr();

                if (way == std::ios_base::beg)
                    new_pos = base + off;
                else if (way == std::ios_base::cur)
                    new_pos = pptr() + off;
                else if (way == std::ios_base::end)
                    new_pos = epptr() + off;

                if (new_pos >= pbase() && new_pos <= epptr()) 
                {
                    pbump(static_cast<int>(new_pos - pptr()));
                    return new_pos - pbase();
                }
            }

            return pos_type(off_type(-1)); // Error
        }

        // Override seekpos for absolute positioning
        pos_type seekpos(pos_type pos, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override 
        {
            return seekoff(pos, std::ios_base::beg, which);
        }

        // Called when the put area is full. For a fixed-size span, this is an overflow error.
        int_type overflow(int_type ch = traits_type::eof()) override { return traits_type::eof(); }

        // Called when the get area is empty. For a fixed-size span, this means we've reached the end.
        int_type underflow() override   { return traits_type::eof(); }

    private:

        void setup_pointers(char* data, size_t size) 
        {
            if (mode & std::ios_base::in) 
                setg(data, data, data + size);

            if (mode & std::ios_base::out) 
                setp(data, data + size);
        }

        std::ios_base::openmode mode;
    };

    /// <summary>
    /// Input span stream. Non-owning span wrapper.
    /// </summary>
    class ISpanStream : public std::istream 
    {
    public:
        ISpanStream(std::span<const char> buffer)
            : std::istream(&spanBuf), spanBuf(buffer, std::ios_base::in)
        { }

        ISpanStream(std::span<char> buffer)
            : std::istream(&spanBuf), spanBuf(buffer, std::ios_base::in) 
        { }

        ISpanStream(Span<const char> buffer)
            : std::istream(&spanBuf), spanBuf(buffer, std::ios_base::in)
        { }

        ISpanStream(Span<char> buffer)
            : std::istream(&spanBuf), spanBuf(buffer, std::ios_base::in)
        { }

        SpanBuffer* rdbuf() const { return const_cast<SpanBuffer*>(&spanBuf); }

    private:
        SpanBuffer spanBuf;
    };

    /// <summary>
    /// Output span stream. Non-owning span wrapper.
    /// </summary>
    class OSpanStream : public std::ostream 
    {
    public:
        OSpanStream(std::span<char> buffer)
            : std::ostream(&spanBuf), spanBuf(buffer, std::ios_base::out) 
        { }

        OSpanStream(Span<char> buffer)
            : std::ostream(&spanBuf), spanBuf(buffer, std::ios_base::out)
        { }

        SpanBuffer* rdbuf() const { return const_cast<SpanBuffer*>(&spanBuf); }

    private:
        SpanBuffer spanBuf;
    };

    /// <summary>
    /// Input/output span stream. Non-owning span wrapper.
    /// </summary>
    class SpanStream : public std::iostream 
    {
    public:
        SpanStream(std::span<char> buffer)
            : std::iostream(&spanBuf), spanBuf(buffer, std::ios_base::in | std::ios_base::out) 
        { }

        SpanStream(Span<char> buffer)
            : std::iostream(&spanBuf), spanBuf(buffer, std::ios_base::in | std::ios_base::out)
        { }

        SpanBuffer* rdbuf() const { return const_cast<SpanBuffer*>(&spanBuf); }

    private:
        SpanBuffer spanBuf;
    };
}