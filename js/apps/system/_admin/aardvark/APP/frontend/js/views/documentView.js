/*jshint browser: true */
/*jshint unused: false */
/*global Backbone, EJS, $, window, arangoHelper, jsoneditor, templateEngine, JSONEditor */
/*global document, _ */

(function() {
  "use strict";

  var createDocumentLink = function(id) {
    var split = id.split("/");
    return "collection/"
      + encodeURIComponent(split[0]) + "/"
      + encodeURIComponent(split[1]);

  };

  window.DocumentView = Backbone.View.extend({
    el: '#content',
    colid: 0,
    docid: 0,

    template: templateEngine.createTemplate("documentView.ejs"),

    events: {
      "click #saveDocumentButton" : "saveDocument",
      "click #deleteDocumentButton" : "deleteDocumentModal",
      "click #confirmDeleteDocument" : "deleteDocument",
      "click #document-from" : "navigateToDocument",
      "click #document-to" : "navigateToDocument",
      "keydown .ace_editor" : "keyPress",
      "keyup .jsoneditor .search input" : "checkSearchBox"
    },

    checkSearchBox: function(e) {
      if ($(e.currentTarget).val() === '') {
        this.editor.expandAll();
      }
    },

    keyPress: function(e) {
      if (e.ctrlKey && e.keyCode === 13) {
        e.preventDefault();
        this.saveDocument();
      }
      else if (e.metaKey && e.keyCode === 13) {
        e.preventDefault();
        this.saveDocument();
      }
    },

    editor: 0,

    setType: function (type) {
      var result, type2;
      if (type === 'edge') {
        result = this.collection.getEdge(this.colid, this.docid);
        type2 = "Edge: ";
      }
      else if (type === 'document') {
        result = this.collection.getDocument(this.colid, this.docid);
        type2 = "Document: ";
      }
      if (result === true) {
        this.type = type;
        this.fillInfo(type2);
        this.fillEditor();
        return true;
      }
    },

    deleteDocumentModal: function() {
      var buttons = [], tableContent = [];
      tableContent.push(
        window.modalView.createReadOnlyEntry(
          'doc-delete-button',
          'Delete',
          'Delete this ' + this.type + '?',
          undefined,
          undefined,
          false,
          /[<>&'"]/
      )
      );
      buttons.push(
        window.modalView.createDeleteButton('Delete', this.deleteDocument.bind(this))
      );
      window.modalView.show('modalTable.ejs', 'Delete Document', buttons, tableContent);
    },

    deleteDocument: function() {

      var result;

      if (this.type === 'document') {
        result = this.collection.deleteDocument(this.colid, this.docid);
        if (result === false) {
          arangoHelper.arangoError('Document error:','Could not delete');
          return;
        }
      }
      else if (this.type === 'edge') {
        result = this.collection.deleteEdge(this.colid, this.docid);
        if (result === false) {
          arangoHelper.arangoError('Edge error:', 'Could not delete');
          return;
        }
      }
      if (result === true) {
        var navigateTo =  "collection/" + encodeURIComponent(this.colid) + '/documents/1';
        window.modalView.hide();
        window.App.navigate(navigateTo, {trigger: true});
      }
    },

    navigateToDocument: function(e) {
      var navigateTo = $(e.target).attr("documentLink");
      if (navigateTo) {
        window.App.navigate(navigateTo, {trigger: true});
      }
    },

    fillInfo: function(type) {
      var mod = this.collection.first();
      var _id = mod.get("_id"),
        _key = mod.get("_key"),
        _rev = mod.get("_rev"),
        _from = mod.get("_from"),
        _to = mod.get("_to");

      $('#document-type').text(type);
      $('#document-id').text(_id);
      $('#document-key').text(_key);
      $('#document-rev').text(_rev);

      if (_from && _to) {

        var hrefFrom = createDocumentLink(_from);
        var hrefTo = createDocumentLink(_to);
        $('#document-from').text(_from);
        $('#document-from').attr("documentLink", hrefFrom);
        $('#document-to').text(_to);
        $('#document-to').attr("documentLink", hrefTo);
      }
      else {
        $('.edge-info-container').hide();
      }
    },

    fillEditor: function() {
      var toFill = this.removeReadonlyKeys(this.collection.first().attributes);
      this.editor.set(toFill);
      $('.ace_content').attr('font-size','11pt');
    },

    jsonContentChanged: function() {
      this.enableSaveButton();
    },

    render: function() {
      $(this.el).html(this.template.render({}));
      this.disableSaveButton();
      this.breadcrumb();

      var self = this;

      var container = document.getElementById('documentEditor');
      var options = {
        change: function(){self.jsonContentChanged();},
        search: true,
        mode: 'tree',
        modes: ['tree', 'code'],
        iconlib: "fontawesome4"
      };
      this.editor = new JSONEditor(container, options);

      return this;
    },

    removeReadonlyKeys: function (object) {
      return _.omit(object, ["_key", "_id", "_from", "_to", "_rev"]);
    },

    saveDocument: function () {
      var model, result;

      if ($('#saveDocumentButton').attr('disabled') === undefined) {

        try {
          model = this.editor.get();
        }
        catch (e) {
          this.errorConfirmation();
          this.disableSaveButton();
          return;
        }

        model = JSON.stringify(model);

        if (this.type === 'document') {
          result = this.collection.saveDocument(this.colid, this.docid, model);
          if (result === false) {
            arangoHelper.arangoError('Document error:','Could not save');
            return;
          }
        }
        else if (this.type === 'edge') {
          result = this.collection.saveEdge(this.colid, this.docid, model);
          if (result === false) {
            arangoHelper.arangoError('Edge error:', 'Could not save');
            return;
          }
        }

        if (result === true) {
          this.successConfirmation();
          this.disableSaveButton();
        }
      }
    },

    successConfirmation: function () {
      $('#documentEditor .tree').animate({backgroundColor: '#C6FFB0'}, 500);
      $('#documentEditor .tree').animate({backgroundColor: '#FFFFF'}, 500);

      $('#documentEditor .ace_content').animate({backgroundColor: '#C6FFB0'}, 500);
      $('#documentEditor .ace_content').animate({backgroundColor: '#FFFFF'}, 500);
    },

    errorConfirmation: function () {
      $('#documentEditor .tree').animate({backgroundColor: '#FFB0B0'}, 500);
      $('#documentEditor .tree').animate({backgroundColor: '#FFFFF'}, 500);

      $('#documentEditor .ace_content').animate({backgroundColor: '#FFB0B0'}, 500);
      $('#documentEditor .ace_content').animate({backgroundColor: '#FFFFF'}, 500);
    },

    enableSaveButton: function () {
      $('#saveDocumentButton').prop('disabled', false);
      $('#saveDocumentButton').addClass('button-success');
      $('#saveDocumentButton').removeClass('button-close');
    },

    disableSaveButton: function () {
      $('#saveDocumentButton').prop('disabled', true);
      $('#saveDocumentButton').addClass('button-close');
      $('#saveDocumentButton').removeClass('button-success');
    },

    breadcrumb: function () {
      var name = window.location.hash.split("/");
      $('#transparentHeader').append(
        '<div class="breadcrumb">'+
        '<a href="#collections" class="activeBread">Collections</a>'+
        '<span class="disabledBread"><i class="fa fa-chevron-right"></i></span>'+
        '<a class="activeBread" href="#collection/' + name[1] + '/documents/1">' + name[1] + '</a>'+
        '<span class="disabledBread"><i class="fa fa-chevron-right"></i></span>'+
        '<a class="disabledBread">' + name[2] + '</a>'+
        '</div>'
      );
    },

    escaped: function (value) {
      return value.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;")
      .replace(/"/g, "&quot;").replace(/'/g, "&#39;");
    }
  });
}());
